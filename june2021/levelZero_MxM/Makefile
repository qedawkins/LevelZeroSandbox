all:
	clang++ -std=c++14 -O0 -fpermissive -rdynamic -fPIC mxm.cpp -o mxm ${ZE_SHARED_LOADER} -L/home/quinn/nod/level-zero/build/lib -lstdc++ -lze_loader 
	clang++ -std=c++14 -O0 -fpermissive -rdynamic -fPIC mxm_static.cpp -o mxmstatic ${ZE_SHARED_LOADER} -L/home/quinn/nod/level-zero/build/lib -lstdc++ -lze_loader 
	clang++ -std=c++14 -O0 -fpermissive -rdynamic -fPIC mxm_fromiree.cpp -o mxmiree ${ZE_SHARED_LOADER} -L/home/quinn/nod/level-zero/build/lib -lstdc++ -lze_loader 
