# Brain Buster by DinoJerks

An application by [Charlie Whitney](http://sharkbox.com) and [Benjamin Bokjo](http://benjaminbojko.com) for #hackthedinos at the AMNH on 11/22/2015 to solve the [Build A Brain challenge](https://github.com/amnh/HacktheDinos/wiki/Build-A-Brain).

It takes in CT scan slices and outputs a mesh cloud based on a selected bone density.

This compiles to an executable application for OSX 10.8 and up.

Pre-compiled executables can be found under [Releases](https://github.com/HackTheDinos/DinoJerks/releases).

Written with [Cinder 0.9.0](https://libcinder.org/)

## Links

* [Presentation](https://docs.google.com/presentation/d/1G-WagRxOPHW9aEhLG6annv1y-Zyt1DyZmsQQMLqao1w/edit?usp=sharing)
* [Screen Recording](https://www.youtube.com/watch?v=hDsgIgJ7ETI)
* [Build A Brain Challenge](https://github.com/amnh/HacktheDinos/wiki/Build-A-Brain)
* [Cinder](https://libcinder.org/)

## Using the App

Once compiled and running, the app can load any sequence of 8-bit TIF, PNG or JPG images. Press `CMD + O` to open a file dialog and select the first image of your sequence. This will load the image in 2D mode.

*Sample data from Stanford University can be downloaded [here](https://graphics.stanford.edu/data/voldata/) (e.g. [8-bit TIFs of human skull](https://graphics.stanford.edu/data/voldata/cthead-8bit.tar.gz))*

Use the slider at the bottom to browse through the individual CT slices. Use the menu on the left to switch to `3D Mode`.

## Development

This project is written in Cinder 0.9.0 and was developed and tested on Mac OS 10.11 (El Capitan) with Xcode 7.1.

The project should run in VS on Windows too, but we haven't tested it or modified the template VS project files.

### Getting Started

Follow the steps below to checkout the app code, Cinder and dependencies and build them.

```bash
# Clone the app repo
git clone git@github.com:HackTheDinos/DinoJerks.git

# Set the working dir
cd DinoJerks

# Check out Cinder and all dependencies
git submodule update --init --recursive

# Build Cinder (this will take a while)
(cd Cinder/xcode; ./fullbuild.sh)

# Open the app project in Xcode
open BrainBuster/xcode/BrainTest.xcodeproj

# Now hit 'Run' (CMD+R) in Xcode
```

## Next Steps

We could use help with any of the following:

* Allow adjustment of resolution per slice
* Ensure that 3D point clouds are centered
* Create 3D mesh and stl file from our program
* Noise filters for older fossils with noisy sediment
* Speed and optimization
