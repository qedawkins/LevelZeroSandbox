__kernel void mxm(__global int* a, __global int* b, __global int *c) {
	uint jdx = get_global_id(0);

    int m = 1;
    int k = 4;
    int n = 3;

	int sum = 0;
	for (int i = 0; i < k; i++) {
		sum += a[i] * b[i * n + jdx];
	}

	c[jdx] = sum;
}
