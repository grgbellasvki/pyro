# IcarusPyro

The IcarusPyro library provides support for computing pyrolysis gas properties
and as a thin-wrapper around the Mutation++ library.

# Thirdparth libraries

The versions of the third-party libraries tested are tagged as git submodules
in the thirdparty subdirectory. To use these versions, then after cloning the
repository, initialize and update the thirdparty source code by

```
git submodule init
git submodule update
```

# Installation Notes

The Mutation++ library does not install a CMake target (it should!), so we
are using a custom FindMPP.cmake routine in the subdirectory cmake/Modules.
To find Mutation we use the environmental variable `MPP_ROOT` and require
that the `lib`, `include`, and `bin` directories are immediate subdirectories
of `MPP_ROOT`. Users may also need to manually copy/install the `data` 
directory of Mutation++ to the `MPP_ROOT` directory. 

