__kernel void mxm(__global int* a, __global int* b, __global int *c, const int m, const int k, const int n) {
	uint idx = get_global_id(0);
	uint jdx = get_global_id(1);

	int sum = 0;
	for (int i = 0; i < k; i++) {
		sum += a[idx * m + i] * b[i * n + jdx];
	}

	c[idx * m + jdx] = sum;
}
