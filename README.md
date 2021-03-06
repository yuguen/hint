# High-level synthesis Integer Library

Hint is an header-only arbitrary size integer API with strong semantic for C++. 
Multiple backends are provided using various HLS library, allowing a user to write 
one operator and synthetising using the main vendor tools.

# Installation 

+ Clone the current repository 
+ Create a build directory 
+ Create a cmake build 
+ Install the headers

```Shell
git clone https://github.com/yuguen/hint.git 
cd hint
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=[INSTALL_PATH] ..
make install
```

The hint headers will be installed in [INSTALL_PATH]/include/hint. 

A libhint cmake package is also exported at [INSTALL_PATH]/include/hint/libhint.cmake.

# Using hint in your project

## Inclusion

### With cmake

If you use cmake on your project, you can use the following line in your CMakeList.txt : 

```CMake
find_package(libhint CONFIG REQUIRED)
```

it will enable a `hint` library target to which you can link your targets.

### Without cmake

add the `-I[INSTALL_PATH]/include/hint` flag to your compiler.

## Activating a backend

Before including the `hint.hpp` header you need to define `INTEL_BACKEND` or `VIVADO_BACKEND` to enable the corresponding wrapper.
As wrappers includes backend specific headers, it avoid the need to have all the tools installed if you only target one of the backend.

# API usage example

Here is a (extremely simple) toy example of a component adding two 17-bits unsigned integer, in order to understand the general hint usage syntax.

The portable architecture is written using the hint API in `hint_comp.hpp`.
```C++
template<template<unsigned int, bool> class Wrapper>
Wrapper<18, false> add17(
	Wrapper<17, false> in0,
	Wrapper<17, false> in1
)
{
	return in0 + in1;
}
```

the idea is to parametrize your operator code with a template template type (called Wrapper in the toy exemple). 
The two parameters of this template template type are the width of the represented integer and its signedness. 
So `in0` and `in1` are defined as two 17 bit unsigned integers of a certain Wrapper type.

To implement a Vivado HLS or Intel HLS version, only the top level component differs, to provide the interface that the tool is waiting for : 

For vivado HLS :
```C++
#include "ap_int.h" 

//enable the VivadoWrapper<> type
#define VIVADO_BACKEND 
#include "hint.hpp"
#include "hint_comp.hpp" 

ap_uint<18> comp(ap_uint<17> in0, ap_uint<17> in1)
{
	VivadoWrapper<17, false> hint_in0{in0};
	VivadoWrapper<17, false> hint_in1{in1};
	
	auto result = add17(hint_in0, hint_in1);

	// Necessary for vivado component
	return result.unravel();	
}
```


for intel HLS :

```C++
#include "ac_int.h"

// enable the IntelWrapper<> type
#define INTEL_BACKEND
#include "hint.hpp"
using namespace ihc;

#include "hint_comp.hpp"

component intel_comp(
		IntelWrapper<17, false> in0,
		IntelWrapper<17, false> in1
	)
{
	return add17(in0, in1);
}
```

# Hint available methods 

- The assignment ```=``` operator: only matching sizes
- The slicing operator ```x.slice<h,l>()```  
- The concatenation operator ```x.concatenate(y)```  
- etc.

# Hint available operators
 
- A shifter: ```shifter```
- A combined shifter+sticky: ```shifter_sticky```
- A leading zero or one counter: ```lzoc```
- A combined leading zero or one counter + shifter: ```LZOC_shift```


# Build Unit Tests
To build the unit tests, go to your build folder and type;

```Shell
cmake <path to CMake_Lists.txt> -DBUILD_UNIT_TEST=ON \
	# for testing the Vivado backend \
	-DVIVADO_INCLUDES=<path to Vivado include dir> \
	# for testing the Intel backend \
	-DINTEL_INCLUDES=<path to IntelFPGA/hls/ include dir> \
	-B . 
make
```

# Reference 

Hint was first presented at the HEART 2019 conference in Nagasaki.
The original article can be found [here](https://hal.archives-ouvertes.fr/hal-02131798).

If you use this library, please cite the following reference : 

```Tex
@inproceedings{forget:hal-02131798,
  TITLE = {{A type-safe arbitrary precision arithmetic portability layer for HLS tools}},
  AUTHOR = {Forget, Luc and Uguen, Yohann and de Dinechin, Florent and Thomas, David},
  URL = {https://hal.inria.fr/hal-02131798},
  BOOKTITLE = {{HEART 2019 - International Symposium on Highly Efficient Accelerators and Reconfigurable Technologies}},
  ADDRESS = {Nagasaki, Japan},
  PAGES = {1-6},
  YEAR = {2019},
  MONTH = Jun,
  DOI = {10.1145/3337801.3337809},
  KEYWORDS = {arithmetic ; arbitrary precision ; HLS ; floating-point},
  PDF = {https://hal.inria.fr/hal-02131798/file/hal_hint.pdf},
  HAL_ID = {hal-02131798},
  HAL_VERSION = {v2},
}
```
