## Installation guide.

### 0. Create the external directory, if it isn't exist yet

To have everything up and running without explicitly configuring \
Makefile, the `external` directory must be created.

```console
$ mkdir external && cd external
```

### 1. Install [CMake](https://cmake.org/)

## TODO

### 2. Install [spdlog](https://github.com/gabime/spdlog)

```console
$ git clone https://github.com/gabime/spdlog.git
$ cd spdlog && mkdir build && cd build
```

The following options force spdlog cmake to: \
  `1.` Build shared library; \
  `2.` Build static or shared library using precompiled header to speed up compilation time; \
  `3.` Build position independent code (-fPIC); \
  `4.` Enable compiler warnings.

```console
$ cmake ../ -DSPDLOG_BUILD_SHARED=ON -DSPDLOG_ENABLE_PCH=ON -DSPDLOG_BUILD_PIC=ON -DSPDLOG_BUILD_WARNINGS=ON

$ make [-j]
```

### 3. Compiling and running `simulation.out`

After this steps, the executable can built successfully. To do so, \
run the following commands from the home directory.

```console
$ make [-j]
```

The binary file will be created in the bin folder. Execution of \
the code should be performed from the home directory too

```console
$ bin/simulation.out
```
