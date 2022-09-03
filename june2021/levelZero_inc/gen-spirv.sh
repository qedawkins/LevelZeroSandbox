clang -cc1 -triple=spir-unknown-unknown addOne.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o addOne.bc
llvm-spirv addOne.bc -o addOne.spv

clang -cc1 -triple=spir-unknown-unknown addOneScalar.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o addOneScalar.bc
llvm-spirv addOneScalar.bc -o addOneScalar.spv
