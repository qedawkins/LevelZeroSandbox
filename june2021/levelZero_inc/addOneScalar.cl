__kernel void addone(__global int* a, __global int* b) {
	b[0] = a[0] + 1;
}
