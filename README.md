# High-level synthesis Integer Library

Use one integer library, run it with any supported HLS tool.

# Usage

Create custom sized integer with the ```hint``` data-type:

```
hint<W, true> var; // signed integer on W bits
hint<W, false> var; // unsigned integer on W bits

var.get(i); // returns the ith bit of var as a hint<1, false>  
var.from_to<Hi, Lo>(); // returns the slice of var from bit index Lo to bit index Hi-1 as a hint<Hi-Lo+1, false>  

```
