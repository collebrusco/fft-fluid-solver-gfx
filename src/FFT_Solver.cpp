#include "FFT_Solver.h"
#include <flgl/logger.h>
LOG_MODULE(fft_solver_base);

FFT_Solver::FFT_Solver(size_t n, float* buff) : N(n), buffer(buff) {}

FFT_Solver::~FFT_Solver() {}

// ======== 1D ========
FFTW_FFT_Solver1d::FFTW_FFT_Solver1d(size_t n, float* buff) : FFT_Solver(n,buff) {
    forw = fftwf_plan_dft_1d(N, (fftwf_complex*)buffer, (fftwf_complex*)buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    inv =  fftwf_plan_dft_1d(N, (fftwf_complex*)buffer, (fftwf_complex*)buffer, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFTW_FFT_Solver1d::~FFTW_FFT_Solver1d() {
    fftwf_destroy_plan(forw);
    fftwf_destroy_plan(inv);
}

void FFTW_FFT_Solver1d::cleanup() {
    fftwf_cleanup();
}

void FFTW_FFT_Solver1d::forward() {
	fftwf_execute(forw);
}

void FFTW_FFT_Solver1d::inverse() {
	fftwf_execute(inv);
}
// ======== 2D ========
FFTW_FFT_Solver2d::FFTW_FFT_Solver2d(size_t n, float* buff) : FFT_Solver(n,buff) {
    forw = fftwf_plan_dft_2d(N, N, (fftwf_complex*)buffer, (fftwf_complex*)buffer, FFTW_FORWARD, FFTW_ESTIMATE);
    inv =  fftwf_plan_dft_2d(N, N, (fftwf_complex*)buffer, (fftwf_complex*)buffer, FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFTW_FFT_Solver2d::~FFTW_FFT_Solver2d() {
    fftwf_destroy_plan(forw);
    fftwf_destroy_plan(inv);
}

void FFTW_FFT_Solver2d::cleanup() {
    fftwf_cleanup();
}

void FFTW_FFT_Solver2d::forward() {
	fftwf_execute(forw);
}

void FFTW_FFT_Solver2d::inverse() {
	fftwf_execute(inv);
}
