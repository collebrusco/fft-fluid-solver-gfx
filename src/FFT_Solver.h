#ifndef FFT_BASE_H
#define FFT_BASE_H
#include <fftw3.h>

/*
	This is the abstract interface for our various FFT solvers
	Can be used to implement 1D or 2D -- make clear in name
	FFTs will be performed on inputs of real #'s represented as float arrays
	the FFTs should be performed in place -- even if this is just a copy :(
	FFT buffer is of size (n*n*2)

	Where for i,j from 0 to N-1:
	real part of elem i,j: buffer[2 * (i+N*j)    ]
	imag part of elem i,j: buffer[2 * (i+N*j) + 1]

	These will be ffts of real data, thus there will be no imaginary part in the input
*/

class FFT_Solver {
protected:
	const size_t N;
	float* const buffer;
public:
	FFT_Solver(size_t n, float* buffer);
	virtual ~FFT_Solver();

	// real -> complex, in place (ok to fake being in place)
	virtual void forward() = 0;

	// complex -> real, in place (ok to fake being in place)
	virtual void inverse() = 0;

};

// example: this solver uses MIT's FFTW for a 1d solver
class FFTW_FFT_Solver1d : public FFT_Solver {
    fftwf_plan forw, inv;
public:
	FFTW_FFT_Solver1d(size_t n, float* buff);
	virtual ~FFTW_FFT_Solver1d();

	static void cleanup(); // global fftw cleanup

	virtual void forward() override final;
	virtual void inverse() override final;

};

// example: this solver uses MIT's FFTW for a 2d solver
class FFTW_FFT_Solver2d : public FFT_Solver {
    fftwf_plan forw, inv;
public:
	FFTW_FFT_Solver2d(size_t n, float* buff);
	virtual ~FFTW_FFT_Solver2d();

	static void cleanup(); // global fftw cleanup

	virtual void forward() override final;
	virtual void inverse() override final;

};

#endif
