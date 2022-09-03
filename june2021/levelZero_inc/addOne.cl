__kernel void addone(__global int* a, __global int* b, const int n) {
	uint idx = get_global_id(0);
	uint jdx = get_global_id(1);

	b[idx * n + jdx] = a[idx * n + jdx] + 1;
}
