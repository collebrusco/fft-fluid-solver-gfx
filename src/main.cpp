#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
#include "SolverToy.h"
#include "FFT_Solver.h"
LOG_MODULE(testmain);
using namespace glm;

#define N (128)

int testmain() {

	FFTW_FFT_Solver2d* fft;
	SolverToy toy(N);

	toy.set_fft_type(fft);

	toy.run();

	return 0;
}
