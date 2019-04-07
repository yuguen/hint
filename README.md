# High-level synthesis Integer Library

Use one integer library, run it with any supported HLS tool.

# Usage
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