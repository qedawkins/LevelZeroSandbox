clang -cc1 -triple=spir-unknown-unknown fill.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o fill.bc
llvm-spirv fill.bc -o fill.spv

clang -cc1 -triple=spir-unknown-unknown fillScalar.cl -O2 -no-opaque-pointers -finclude-default-header -emit-llvm-bc -o fillScalar.bc
llvm-spirv fillScalar.bc -o fillScalar.spv
