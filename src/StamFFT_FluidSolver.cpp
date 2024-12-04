#include "StamFFT_FluidSolver.h"
#include <flgl/logger.h>
#include <random>
#include <cstring>
LOG_MODULE(fft_solver)

#define __floor(x) ((x)>=0.0?((int)(x)):(-((int)(1-(x)))))
	
// === private members: ===
// const int n;
// fftwf_plan forward_u, forward_v, inv_u, inv_v;
// float* u, * v, * u0, * v0;
// float visc;
// float* buffer;

StamFFT_FluidSolver::StamFFT_FluidSolver(int const& N) : n(N), 
                                                         viscosity(visc), 
                                                         force_mul(1.) {
	visc = 0.001;
    alloc_buffers();
    initFFT(n, u0, v0);
}
StamFFT_FluidSolver::~StamFFT_FluidSolver() {
    destroyFFT();
    free_buffers();
}

float* StamFFT_FluidSolver::buff() const {return buffer;}
float* StamFFT_FluidSolver::x_buffer() const {return u;}
float* StamFFT_FluidSolver::y_buffer() const {return v;}
int const& StamFFT_FluidSolver::dim() const {return n;}

void StamFFT_FluidSolver::add_force(int x, int y, int fx, int fy) {
	u0[x+y*n] += fx * force_mul;
	v0[x+y*n] += fy * force_mul;
}

void StamFFT_FluidSolver::set_force(int x, int y, int fx, int fy) {
	u0[x+y*n] = fx * force_mul;
	v0[x+y*n] = fy * force_mul;
}

void StamFFT_FluidSolver::get_force(int x, int y, int& fx, int& fy) const {
	fx = u0[x+y*n] / force_mul;
	fy = v0[x+y*n] / force_mul;
}

void StamFFT_FluidSolver::set_force_multiplier(float mul) {
    force_mul = mul;
}

float StamFFT_FluidSolver::force_multiplier() {
    return force_mul;
}

void StamFFT_FluidSolver::initFFT(int const& N, float* u_buffer, float* v_buffer) {
    forward_u = fftwf_plan_dft_r2c_2d(n, n, u_buffer, (fftwf_complex*)u_buffer, FFTW_ESTIMATE);
    forward_v = fftwf_plan_dft_r2c_2d(n, n, v_buffer, (fftwf_complex*)v_buffer, FFTW_ESTIMATE);
    inv_u =     fftwf_plan_dft_c2r_2d(n, n, (fftwf_complex*)u_buffer, u_buffer, FFTW_ESTIMATE);
    inv_v =     fftwf_plan_dft_c2r_2d(n, n, (fftwf_complex*)v_buffer, v_buffer, FFTW_ESTIMATE);
}
void StamFFT_FluidSolver::alloc_buffers() {
    u = new float[n*n];
    v = new float[n*n];
    u0 = new float[n*(n+2)];
    v0 = new float[n*(n+2)];
    buffer = new float[n*n*2];
}
void StamFFT_FluidSolver::free_buffers() {
    delete [] u;
    delete [] v;
    delete [] u0;
    delete [] v0;
    delete [] buffer;
}
void StamFFT_FluidSolver::destroyFFT() {
    fftwf_destroy_plan(forward_u);
    fftwf_destroy_plan(forward_v);
    fftwf_destroy_plan(inv_u);
    fftwf_destroy_plan(inv_v);
    fftwf_cleanup();
    LOG_DBG("destroyed fftw");
}

void StamFFT_FluidSolver::random_fill(float mag) { 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-mag, mag);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            u0[i+j*n] = dis(gen); v0[i+j*n] = dis(gen);
        }
    }
}

void StamFFT_FluidSolver::zero_field() {
    memset(u,0,sizeof(float)*n*n);
    memset(v,0,sizeof(float)*n*n);
}

void StamFFT_FluidSolver::slow_fill_pixbuff() {
	float buffmax = -9999999.f;
	float buffmin = 9999999.f;
	for (int i = 0; i < n; i++) {
		int x = i - (n/2);
		for (int j = 0; j < n; j++) {
			int y = j - (n/2);
			int r = 2*(i+j*n);
			int g = 2*(i+j*n) + 1;
			buffer[r] = u[i+j*n];
			buffer[g] = v[i+j*n];
			if (buffer[r]>buffmax) buffmax = buffer[r];
			if (buffer[g]>buffmax) buffmax = buffer[g];
			if (buffer[r]<buffmin) buffmin = buffer[r];
			if (buffer[g]<buffmin) buffmin = buffer[g];
		}
	}
	// for (int i = 0; i < n; i++) {
	// 	for (int j = 0; j < n; j++) {
	// 		int r = 2*(i+j*n);
	// 		int g = 2*(i+j*n) + 1;
	// 		buffer[r] -= buffmin;
	// 		buffer[g] -= buffmin;
    //         buffer[r] = log2(buffer[r]);
    //         buffer[g] = log2(buffer[g]);
    //         buffmax -= buffmin;
    //         if (buffmax < 1.f) buffmax = 2.f;
    //         buffer[r] *= (1.f / log2(buffmax));
    //         buffer[g] *= (1.f / log2(buffmax));
	// 	}
	// }
    for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int r = 2*(i+j*n);
			int g = 2*(i+j*n) + 1;
			float imag = (1.f / ((buffmax-buffmin)==0.?1.f:(buffmax-buffmin)));
			buffer[r] -= buffmin;
			buffer[g] -= buffmin;
			buffer[r] *= imag;
			buffer[g] *= imag;
		}
	}
}

void StamFFT_FluidSolver::step(float const& dt) {
	this->stam_stable_solve(n, u, v, u0, v0, visc, dt);
}

/*
	This is Stam's solver itself
	It is almost the same as in his paper
	but adapted to use fftw3 and commented
	
	Inputs:
		n: 			size of n by n field
		(u, v): 	velocity of the prev time step
		(u0, v0): 	a force field defined on a grid
		visc: 		viscosity of fluid
		dt: 		time step
	Outputs:
		(u, v): 	next step of fluid solver
		(u0,v0):	cleared for new forces
*/
void StamFFT_FluidSolver::stam_stable_solve(int const& N, 
                       float* const u, float* const v, 
                       float* const u0, float* const v0, 
                       float const& visc, float const& dt)
{
    float x, y, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    // apply forces
    for ( i=0 ; i<N*N ; i++ ) {
        u[i] += dt*u0[i]; u0[i] = u[i];
        v[i] += dt*v0[i]; v0[i] = v[i];
    }

    // self-advection: semi-Lagrangian scheme
    // here, (u0,v0) are used to interpolate 
    // and (u,v) stores the interpolation & result
    for ( i=0 ; i<N ; i++ ) {
        for ( j=0 ; j<N ; j++ ) {
            x = i-dt*u0[i+N*j]*N; y = j-dt*v0[i+N*j]*N;
            i0 = __floor(x); s = x-i0; i0 = (N+(i0%N))%N; i1 = (i0+1)%N;
            j0 = __floor(y); t = y-j0; j0 = (N+(j0%N))%N; j1 = (j0+1)%N;
            u[i+N*j] = (1-s)*((1-t)*u0[i0+N*j0]+t*u0[i0+N*j1])+
                          s *((1-t)*u0[i1+N*j0]+t*u0[i1+N*j1]);
            v[i+N*j] = (1-s)*((1-t)*v0[i0+N*j0]+t*v0[i0+N*j1])+
                          s *((1-t)*v0[i1+N*j0]+t*v0[i1+N*j1]);
        }
    }

    // copy velos into real-part of fourier buffers (i think)
    for ( i=0 ; i<N ; i++ ) {
        for ( j=0 ; j<N ; j++ ) {
        	u0[i+(N+2)*j] = u[i+N*j]; 
        	v0[i+(N+2)*j] = v[i+N*j]; 
        }
    }

    // transform to fourier domain
    fftwf_execute(forward_u); fftwf_execute(forward_v);

    // apply low pass filter to simulate viscosity
    // and force field to be mass converving
    // by projecting vectors onto line perpendicular to wave #
    // which is line tan to circles centered at origin
    for ( i=0 ; i<=N ; i+=2 ) {
        x = 0.5*i;
        for ( j=0 ; j<N ; j++ ) {
            y = j<=N/2 ? j : j-N;
            r = x*x+y*y;
            if ( r==0.0 ) continue;
            f = exp(-r*dt*visc); // filter
            U[0] = u0[i  +(N+2)*j]; V[0] = v0[i  +(N+2)*j];
            U[1] = u0[i+1+(N+2)*j]; V[1] = v0[i+1+(N+2)*j];	// transformed complex vector (U,V)
            u0[i  +(N+2)*j] = f*( (1-x*x/r)*U[0]     -x*y/r *V[0] );
            u0[i+1+(N+2)*j] = f*( (1-x*x/r)*U[1]     -x*y/r *V[1] );
            v0[i+  (N+2)*j] = f*(   -y*x/r *U[0] + (1-y*y/r)*V[0] );
            v0[i+1+(N+2)*j] = f*(   -y*x/r *U[1] + (1-y*y/r)*V[1] ); // apply filter & project
        }
    }

    // inverse ffts back to spatial domain
    fftwf_execute(inv_u); fftwf_execute(inv_v);

    // normalize (r2c then c2r tform multiplies all by n*n)
    f = 1.0/(N*N);
    for ( i=0 ; i<N ; i++ ) {
        for ( j=0 ; j<N ; j++ )
        { 
            u[i+N*j] = f*u0[i+(N+2)*j]; v[i+N*j] = f*v0[i+(N+2)*j]; 
        }
    }

    // clear force field
    memset(u0,0,sizeof(float)*N*N);
    memset(v0,0,sizeof(float)*N*N);

}
