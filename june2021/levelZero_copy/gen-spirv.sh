clang -cc1 -triple=spir-unknown-unknown copy.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o copy.bc
llvm-spirv copy.bc -o copy.spv
