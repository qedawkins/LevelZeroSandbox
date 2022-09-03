
clang -cc1 -triple=spir-unknown-unknown matrixMultiply.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o matrixMultiply.bc
llvm-spirv matrixMultiply.bc -o matrixMultiply.spv
