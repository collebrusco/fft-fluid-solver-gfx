#ifndef SOLVER_TOY
#define SOLVER_TOY

#include <stdint.h>
#include <type_traits>
#include <flgl/logger.h>
#include "Field.h"
#include "FieldRenderer.h"
#include "rgFieldRenderer.h"
#include "vecFieldRenderer.h"
#include "FFT_Solver.h"

// Entire fluid sim & gfx demo wrapped up into an object
// usage:
// SolverToy toy(128); yourFFT* fft; toy.set_fft_type(fft); toy.run();
// extend if you want to add code to each loop with userLoop

class SolverToy {
private:
	// called every frame, override if you need/want extra functionality
	virtual void userLoop(float dt);
	FFT_Solver * fftu, * fftv;
protected:
	const size_t N;
	Field field;
	FieldRenderer * rendererA, * rendererB;
	void explode(int x0, int y0, int r, float force=20000.);
public:
	// solver will run n x n simulation, MUST BE POW OF TWO
	SolverToy(size_t n);
	virtual ~SolverToy();

	// pass a null pointer to an FFT subclass 
    template <typename Solver_t>
    void set_fft_type(Solver_t * fft) {
        static_assert(std::is_base_of<FFT_Solver, Solver_t>::value, 
                      "Must pass a ptr to an FFT_Solver2d derivative");
        delete fftu; delete fftv;
        fftu = new Solver_t(N, field.solver.fx_buffer());
        fftv = new Solver_t(N, field.solver.fy_buffer());
        field.solver.use_ffts(fftu, fftv);
    }

	// enter main loop, runs until esc pressed or win closed
	void run(void(*loopfunc)(float) = 0);

};

#endif
