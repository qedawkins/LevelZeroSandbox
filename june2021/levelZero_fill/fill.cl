__kernel void fill(__global int* a, const int n) {
	uint idx = get_global_id(0);
	uint jdx = get_global_id(1);

	a[idx * n + jdx] = 5;
}
