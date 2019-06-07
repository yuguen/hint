# High-level synthesis Integer Library

Use one integer library, run it with any supported HLS tool.

# Hint Usage
The user should write his C++ operator such that it is templated by a wrapper with the following signature:
```
template<..., template<unsigned int , bool> class Wrapper, ...>
... operator(...){
	...
}
``` 

The operator can the be instantiated using the correct Hint wrapper (either ```VivadoWrapper``` or ```IntelWrapper```) included in the ```hint.hpp``` header. 

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