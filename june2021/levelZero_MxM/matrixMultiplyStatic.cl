__kernel void mxm(__global int* a, __global int* b, __global int *c) {
	uint idx = get_global_id(0);
	uint jdx = get_global_id(1);

    int m = 1;
    int k = 4;
    int n = 3;

	int sum = 0;
	for (int i = 0; i < k; i++) {
		sum += a[idx * m + i] * b[i * n + jdx];
	}

	c[idx * m + jdx] = sum;
}
