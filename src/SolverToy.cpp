#include "SolverToy.h"
#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
#include <Stopwatch.h>
#include <random>
#include "Stepper.h"
#include "StamFFT_FluidSolver.h"
#include "Field.h"
#include "FieldRenderer.h"
#include "rgFieldRenderer.h"
#include "vecFieldRenderer.h"
#include "SolverToy.h"
LOG_MODULE(solvertoy);
using namespace glm;

SolverToy::SolverToy(size_t n) : N(n), field(n) {
	LOG_DBG("creating default ffts..");
    fftu = new FFTW_FFT_Solver2d(n, field.solver.fx_buffer());
    fftv = new FFTW_FFT_Solver2d(n, field.solver.fy_buffer());
    LOG_DBG("done creating new ffts");
    field.solver.use_ffts(fftu, fftv);
}
SolverToy::~SolverToy() {
	delete fftu; delete fftv;
}

void SolverToy::userLoop(float dt) {(void)dt;} // unimplemented by default

class debug_buffer {
	const unsigned int N;
	float * buffS, * buffF, * buffR;
	unsigned int i;
public:
	debug_buffer(unsigned int n) : N(n) {
		buffS = new float[N]; buffF = new float[N]; buffR = new float[N]; i = 0;
	}
	~debug_buffer() {
		delete [] buffS; delete [] buffF; delete [] buffR;
	}
	void outavg() {
		float aS = 0., aF = 0., aR = 0.;
		for (int j = 0; j < N; j++) {
			aS += buffS[j];
			aF += buffF[j];
			aR += buffR[j];
		}
		LOG_DBG("solver avg t = %.1fus, ffts avg t = %.1fus for %.1f%% of solver",aS/N,aF/N,100.*(aF/aS));
		LOG_DBG("render avg t = %.1fus", aR/N);
	}
	void dump(float solver, float fft, float render) {
		buffS[i] = solver;
		buffF[i] = fft;
		buffR[i] = render;
		i++;
		if (!(i < N)) {
			i = 0;
			outavg();
		}
	}
	void flush() {
		i = 0;
	}
};

void SolverToy::explode(int x0, int y0, int r, float force) {
	const float mag = 1000.;
	for (int y = y0-r; y < y0+r; y++) {
		int dy = y-y0;
		for (int x = x0-r; x < x0+r; x++) {
			int dx = x-x0;
			if (dx*dx+dy*dy <= r*r) {
				int dx = x-x0; int dy = y-y0;
				float fx = dx * (force/(dx*dx+dy*dy)) * field.solver.force_multiplier(); 
				float fy = dy * (force/(dx*dx+dy*dy)) * field.solver.force_multiplier();
				field.solver.add_force(x,y, fx,fy);
			}
		}
	}
}

#include <iostream>
using namespace std;
void SolverToy::run(void(*loopfunc)(float)) {
	gl.init();
	window.create("fluid solver toy", 1024, 1024);
	Stopwatch timer(SECONDS); timer.start();
	Stopwatch dtimer(SECONDS); dtimer.start();

	rendererA = new rgFieldRenderer(field);
	rendererB = new vecFieldRenderer(field);
	
	field.solver.random_fill(400.);

	rendererA->init();
	rendererB->init();

	rendererA->prepare();
	rendererB->prepare();

	debug_buffer debug(256);	
	bool slowmo = false;
	int view = 0;
	LOG_DBG("entering loop...");
	while (!window.should_close()) {
			// controls
		if (window.keyboard[GLFW_KEY_ESCAPE].down) window.close();
		if (window.keyboard[GLFW_KEY_R].down)
			field.solver.random_fill(400.);
		if (window.keyboard[GLFW_KEY_S].pressed) {
			slowmo = !slowmo;
			field.solver.set_force_multiplier(1. + ((float)(slowmo*9)));
		}
		if (window.keyboard[GLFW_KEY_0].pressed)
			field.solver.zero_field();
		if (window.keyboard[GLFW_KEY_UP].pressed) {
			field.solver.viscosity *= 2.; LOG_DBG("viscosity: %e", field.solver.viscosity);
		}
		if (window.keyboard[GLFW_KEY_DOWN].pressed) {
			field.solver.viscosity /= 2.; LOG_DBG("viscosity: %e", field.solver.viscosity);
		}
		if (window.mouse.left.pressed && window.keyboard[GLFW_KEY_SPACE].down)
			explode(field.mouse_pos().x, field.mouse_pos().y, 128);
		if (window.keyboard[GLFW_KEY_V].pressed) {
			static const char* const rennames[4] = {"color", "vector", "overlay", "using renderer "};
			view = (view+1)%3; debug.flush(); LOG_DBG("%s%s", rennames[3], rennames[view]);
		}
		if (window.keyboard[GLFW_KEY_P].pressed) {
			rendererA->destroy();
			rendererB->destroy();
			rendererA->init();
			rendererB->init();
			rendererA->prepare();
			rendererB->prepare();
		}

			// solver step
		float dt = slowmo ? dtimer.stop_reset_start()/10. : dtimer.stop_reset_start(); dt = 1./60.;
		field.step(dt);
		if (view!=1) field.solver.slow_fill_pixbuff();

			// render
		float st = timer.read(MICROSECONDS);
		gl.clear();
		switch(view) {
		case 2:
			rendererA->prepare();
			rendererA->render();
		case 1:
			rendererB->prepare();
			rendererB->render();
			break;
		case 0:
			rendererA->prepare();
			rendererA->render();
			break;
		}
		float en = timer.read(MICROSECONDS);
		debug.dump(field.solver.get_prev_solver_t(),
				   field.solver.get_prev_fft_t(),
				   en-st);

		this->userLoop(dt);
		if (loopfunc) loopfunc(dt);
		// while (dtimer.read() < 1.);
		window.update();
		// cin.get();
	}

	rendererA->destroy();
	delete rendererA;
	rendererB->destroy();
	delete rendererB;
	gl.destroy();
}








