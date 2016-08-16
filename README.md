# Boiler
Boiler 2D/3D game engine

## Quick Setup
### Clone the repository
```
git clone git@github.com:nickenchev/boiler.git
```

### Prepare the build directory
```
cd boiler
mkdir build
cd build
```

### Setup the build system (Ninja or make)
```
cmake -GNinja ../
ninja
```

### Run the engine
```
cd ..
build/bin/engine
```
You need to run the engine executable from the root directory as the engine uses the data folder/

## Android Setup
### Link in the SDL2 libraries
```
cd android/ndk
ln -s [path-to-SDL2] .
ln -s [path-to-SDL2_image] .
cd ..
ndk-build
```
