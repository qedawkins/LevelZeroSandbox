
clang -cc1 -triple=spir-unknown-unknown matrixMultiply.cl -O0 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o matrixMultiply.bc
llvm-spirv matrixMultiply.bc -o matrixMultiply.spv

clang -cc1 -triple=spir-unknown-unknown matrixMultiplyStatic.cl -O0 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o matrixMultiplyStatic.bc
llvm-spirv matrixMultiplyStatic.bc -o matrixMultiplyStatic.spv

clang -cc1 -triple=spir-unknown-unknown matrixMultiplyStaticNoIdx.cl -O0 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o matrixMultiplyStaticNoIdx.bc
llvm-spirv matrixMultiplyStaticNoIdx.bc -o matrixMultiplyStaticNoIdx.spv
