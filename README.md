# fft fluid solver toy
### Controls
* Click and drag to swirl fluid around
* Space + Click for an explosion
* Press up/down to double/halve viscosity   
* Press R to randomize velocity field
* Press S to toggle slow motion mode
* Press 0 to zero velocity field
* Press V to change view

![gif](/img/product.gif)

I've been wanting to implement a 2d fluid sim like this for some [other projects](https://github.com/collebrusco/gunpowder) and of course the computational fluid dynamics rabbit hole is deep.       
I found [this paper by Jos Stam (2001)](https://www.dgp.toronto.edu/public_user/stam/reality/Research/pdf/jgt01.pdf) that details a relatively simple FFT-based technique.   

Here I've implemented it with [MIT's FFTW](https://www.fftw.org/) FFT library, rendered the fluid field and added some controls for playing with it.        
There are two renderers that you can switch between or overlay: one simply renders the x and y coords of each vector in the red and green components of color, and the other renders the vectors as lines.       
The color renderer surprisingly has a very neat and fluid look to it, while the lines give you the traditional vector field rendering.

# Building
Like any of my graphics projects, this will build on mac windows or linux via cmake. More on that in flgl's [guide](https://github.com/collebrusco/flgl/blob/main/user/README.md).   
     
**However** this code depends on fftw as well. FFTW will install itself as a static library somewhere dependent on OS (/usr/local/lib on unix, somewhere on windows). CMake will search for it in the default locations as well as in lib/fftw/bin relative to this repo. Included there by default is fftw3 built for windows x86 and my system (M1 arm64) so you do not need to bother installing fftw if you use either of those systems. Be sure to build fftw with single precision FFTs.

