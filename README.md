


# Convolution Neural Network Processor

A C++ model of a convolutionnal neural network accelerator on FPGA

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Cloning the project

First downlaod the project on your machine:
```
git clone [Project URL]
```

### Prerequisites

All dependencies are already in the project folder lib/, so no need to install anything.

This project is heavily dependent on the [libfi](https://github.com/gsarkis/libfi) library. libfi allow the simulation of fixed point number. It is in template form to allow easy variable type swap. Been Templatized, you will find most of the modules code in the include/.hpp files.

The automated tests are made with the help of the [Google Test library](https://github.com/google/googletest).

### Compiling

The compiling is done withe the help of [cmake](https://cmake.org/). This command executed at the project root (CNNP/) should build everything for you.
```
cmake
```

## Running the tests

The test can be found in src/test folder. There is one test file for every module containning unit tests for this module. Executing one should run all the test for that module.

To add tests, the [Google Test](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md) framework need to be understood. The google test suite is open source and can be found [HERE](https://github.com/google/googletest)

## Using the processor model

TODO 

## Coding guidelines

* [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) have been used.
* For comments, [doxygen](http://www.stack.nl/~dimitri/doxygen/) commands have been used.

## Versioning

We use [Git](https://git-scm.com/) for versioning.

## Authors

* **Thierry Pouplier** - *Initial work*

## Acknowledgments

* Thanks to François Leduc-Primeau for helping me with this project
* Sze, V., Chen, Y., Yang, T. and Emer, J. (2017). Efficient Processing of Deep Neural Networks: A Tutorial and Survey. Proceedings of the IEEE, 105(12), pp.2295-2329.
* Sankaradas, M., Jakkula, V., Cadambi, S., Chakradhar, S., Durdanovic, I., Cosatto, E., & Graf, H. P. (2009). A Massively Parallel Coprocessor for Convolutional Neural Networks, 53–60. https://doi.org/10.1109/ASAP.2009.25

