## Installation guide.

### -1. Create the external directory, if it isn't exist yet

To have everything up and running without explicitly configuring \
Makefile, the `external` directory must be created.

```console
$ mkdir external && cd external
```

### 0. Install [CMake](https://cmake.org/)

## TODO

### 1. Install [HDF5](https://portal.hdfgroup.org/display/support)

The build had been tested with the latest 1.10 HDF5 version. First, download \
the CMake archive from [this page](https://portal.hdfgroup.org/display/support/Downloads), unzip it in the `external/HDF5` and run \
`build-unix.sh` script. After this, the built binary will be in the build directory \
and will also be copied to the CMake-hdf5-N directory if successful. It will \
have the format:

```
HDF5-N-Linux.tar.gz
```

After that, unzip this archive and put the `HDF5-N-Linux/HDF_Group` \
folder into the `external/HDF5`.

### 2. Install [openPMD](https://github.com/openPMD/openPMD-api)

The detailed information is in the openPMD [sources](https://openpmd-api.readthedocs.io/en/0.14.5/install/install.html).

```console
$ git clone https://github.com/openPMD/openPMD-api.git
$ cd openPMD-api && mkdir build && cd build
```


To take a closer look on build options, visit [this page](https://openpmd-api.readthedocs.io/en/0.14.5/dev/buildoptions.html#development-buildoptions). The following options will: \
  `1.` Install the library in the openPMD-api; \
  `2.` Add the configured HDF5 library to search path; \
  `3.` Force openPMD to use HDF5 (without MPI) and Python; \
  `4.` Enable internal VERIFY (assert) macro independent of build type; \
  `5.` Add installation targets; \
  `6.` Provide the path to the Python executable.

```console
$ cmake ../ -DCMAKE_INSTALL_PREFIX=../ -DCMAKE_PREFIX_PATH=../HDF5/HDF_Group/HDF5/1.10.9:$CMAKE_PREFIX_PATH -DopenPMD_USE_MPI=OFF -DopenPMD_USE_HDF5=ON -DopenPMD_USE_PYTHON=ON -DopenPMD_USE_VERIFY=ON -DopenPMD_INSTALL=ON -DPython_EXECUTABLE=$(which python)

$ cmake --build .

# optional
$ ctest

# sudo might be required for system paths
$ cmake --build . --target install
```

### 3. Install [spdlog](https://github.com/gabime/spdlog)

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

### 4. Compiling and running `simulation.out`

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