## Link to article:

[https://jjfumero.github.io/posts/2021/09/introduction-to-level-zero](https://jjfumero.github.io/posts/2021/09/introduction-to-level-zero/)


## How to compile and run?

```bash
export LEVEL_ZERO_ROOT=/path/to/level-zero-code 
export ZE_SHARED_LOADER=$LEVEL_ZERO_ROOT/build/lib/libze_loader.so
./sources.sh
make
./gen-spirv-sh   ## Generate the SPIR-V code from the OpenCL kernel using CLANG and LLVM
./addone
./addonescalar
```
