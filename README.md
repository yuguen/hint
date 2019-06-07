# High-level synthesis Integer Library

Hint is an header-only arbitrary size integer API with strong semantic for C++. 
Multiple backends are provided using various HLS library, allowing a user to write 
one operator and synthetising using the main vendor tools.

# Example

Here is a (extremely simple) toy example of a component adding two 17-bits unsigned integer, in order to understand the general hint usage syntax.

```
// file hint_comp.hpp

template<template<unsigned int, bool> class Wrapper>
Wrapper<18, false> add17(
	Wrapper<17, false> in0,
	Wrapper<17, false> in1
)
{
	return in0 + in1;
}
```

the idea is to parametrize your operator code with a template template parameter (called Wrapper in the toy exemple). 
The two parameters of this template template type is the width of the represented integer and its signedness. 
So `in0` and `in1` are defined as two 17 bit unsigned integers of a certain type.

To implement a Vivado HLS or Intel HLS version, only the top level component differs, to provide the interface that the tool is waiting for : 

For vivado HLS :
```
#include "ap_int.h" 
#include "hint.hpp"
#include "hint_comp.hpp" //

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

```
#include "hint.hpp"
#include "ac_int.h"
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

```
cmake <path to CMake_Lists.txt> -DBUILD_UNIT_TEST=ON 
	# for testing the Vivado backend
	-DVIVADO_BACKEND=ON -DVIVADO_INCLUDES=<path to Vivado include dir>
	# for testing the Intel backend
	-DINTEL_BACKEND=ON -DINTEL_INCLUDES=<path to IntelFPGA/hls/ include dir>
	-B .
make
```
