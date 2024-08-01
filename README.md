# fft fluid solver toy
### Controls
* Click and drag to swirl fluid around
* Space + Click for an explosion
* Press up/down to double/halve viscosity   
* Press R to randomize velocity field
* Press S to toggle slow motion mode
* Press 0 to zero velocity field
* Press V to change view

![screen shot](/screenshot_vec.png) ![screen shot](/screenshot_clr.png) 

I've been wanting to implement a 2d fluid sim like this for some [other projects](https://github.com/collebrusco/gunpowder) and of course the computational fluid dynamics rabbit hole is deep.       
I found [this paper by Jos Stam (2001)](https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/jgt01.pdf) that details a relatively simple FFT-based technique.   

Here I've implemented it with [MIT's FFTW](https://www.fftw.org/) FFT library, rendered the fluid field and added some controls for playing with it.      
For a school project, a team and I implemented our own FFTs in CUDA! Check that [here](https://github.com/collebrusco/361C-term-project), if you have an NVIDIA GPU this makes the solver faster.     
There are two renderers that you can switch between or overlay: one simply renders the x and y coords of each vector in the red and green components of color, and the other renders the vectors as lines.       
The color renderer surprisingly has a very neat and fluid look to it, while the lines give you the traditional vector field rendering.

# Building
Like any of my graphics projects, this will build on mac windows or linux via cmake. More on that in flgl's [guide](https://github.com/collebrusco/flgl/blob/main/user/README.md).   
     
This code depends on fftw as well. This repo includes two fftw binaries, one built on windows x86 and the other on macos arm64. CMake should find these and build out of the box if you have one of those systems, otherwise [download fftw](https://www.fftw.org/fftw-3.3.10.tar.gz) and install with `./configure --enable-float CC="clang"`, `make` & `sudo make install` to generate libfftw3.a (or libfftw3f.a). Place it in lib/fftw/bin/ as `libfftw3.a` or `libfftw3w.a` on windows and CMake should find it.
