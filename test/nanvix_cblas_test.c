/*
 * nanvix_cblas_test.c - Simple CBLAS functional test for Nanvix
 *
 * Copyright(c) The Maintainers of Nanvix.
 * Licensed under the MIT License.
 *
 * This test verifies basic CBLAS functionality on Nanvix.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../cblas.h"

#define N 4
#define EPSILON 1e-6

/* Helper to check float equality */
static int float_eq(float a, float b) {
    return fabs(a - b) < EPSILON;
}

/* Test CBLAS Level 1: saxpy (y = alpha*x + y) */
static int test_saxpy(void) {
    float x[N] = {1.0f, 2.0f, 3.0f, 4.0f};
    float y[N] = {1.0f, 1.0f, 1.0f, 1.0f};
    float alpha = 2.0f;
    float expected[N] = {3.0f, 5.0f, 7.0f, 9.0f};
    int i;

    printf("  Test saxpy: ");
    cblas_saxpy(N, alpha, x, 1, y, 1);

    for (i = 0; i < N; i++) {
        if (!float_eq(y[i], expected[i])) {
            printf("FAILED (y[%d]=%.2f, expected %.2f)\n", i, y[i], expected[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 1: sdot (dot product) */
static int test_sdot(void) {
    float x[N] = {1.0f, 2.0f, 3.0f, 4.0f};
    float y[N] = {1.0f, 1.0f, 1.0f, 1.0f};
    float result;
    float expected = 10.0f; /* 1+2+3+4 */

    printf("  Test sdot: ");
    result = cblas_sdot(N, x, 1, y, 1);

    if (!float_eq(result, expected)) {
        printf("FAILED (result=%.2f, expected %.2f)\n", result, expected);
        return 1;
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 1: snrm2 (Euclidean norm) */
static int test_snrm2(void) {
    float x[N] = {1.0f, 2.0f, 2.0f, 0.0f};
    float result;
    float expected = 3.0f; /* sqrt(1+4+4+0) = 3 */

    printf("  Test snrm2: ");
    result = cblas_snrm2(N, x, 1);

    if (!float_eq(result, expected)) {
        printf("FAILED (result=%.2f, expected %.2f)\n", result, expected);
        return 1;
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 1: sscal (scale vector) */
static int test_sscal(void) {
    float x[N] = {1.0f, 2.0f, 3.0f, 4.0f};
    float alpha = 2.0f;
    float expected[N] = {2.0f, 4.0f, 6.0f, 8.0f};
    int i;

    printf("  Test sscal: ");
    cblas_sscal(N, alpha, x, 1);

    for (i = 0; i < N; i++) {
        if (!float_eq(x[i], expected[i])) {
            printf("FAILED (x[%d]=%.2f, expected %.2f)\n", i, x[i], expected[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 1: scopy */
static int test_scopy(void) {
    float x[N] = {1.0f, 2.0f, 3.0f, 4.0f};
    float y[N] = {0.0f, 0.0f, 0.0f, 0.0f};
    int i;

    printf("  Test scopy: ");
    cblas_scopy(N, x, 1, y, 1);

    for (i = 0; i < N; i++) {
        if (!float_eq(y[i], x[i])) {
            printf("FAILED (y[%d]=%.2f, expected %.2f)\n", i, y[i], x[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 1: isamax (index of max absolute value) */
static int test_isamax(void) {
    float x[N] = {1.0f, -5.0f, 3.0f, 2.0f};
    CBLAS_INDEX result;
    CBLAS_INDEX expected = 1; /* index of -5.0 (abs value 5.0) */

    printf("  Test isamax: ");
    result = cblas_isamax(N, x, 1);

    if (result != expected) {
        printf("FAILED (result=%d, expected %d)\n", (int)result, (int)expected);
        return 1;
    }
    printf("OK\n");
    return 0;
}

/* Test CBLAS Level 2: sgemv (matrix-vector multiply) */
static int test_sgemv(void) {
    /* A = [1 2; 3 4], x = [1; 1], y = alpha*A*x + beta*y */
    float A[4] = {1.0f, 3.0f, 2.0f, 4.0f}; /* column-major */
    float x[2] = {1.0f, 1.0f};
    float y[2] = {0.0f, 0.0f};
    float expected[2] = {3.0f, 7.0f}; /* A*x = [1+2, 3+4] = [3, 7] */
    int i;

    printf("  Test sgemv: ");
    cblas_sgemv(CblasColMajor, CblasNoTrans, 2, 2, 1.0f, A, 2, x, 1, 0.0f, y, 1);

    for (i = 0; i < 2; i++) {
        if (!float_eq(y[i], expected[i])) {
            printf("FAILED (y[%d]=%.2f, expected %.2f)\n", i, y[i], expected[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

/*
 * Note: Level 3 BLAS (sgemm, dgemm) tests are disabled for P2/P5/P6 targets
 * because the minimal x86 kernel configuration doesn't include the required
 * incopy/itcopy routines. These can be enabled for targets with full GEMM support.
 */
#if !defined(SKIP_LEVEL3_TESTS)
/* Test CBLAS Level 3: sgemm (matrix-matrix multiply) */
static int test_sgemm(void) {
    /* C = alpha*A*B + beta*C where A=B=I (2x2 identity) */
    float A[4] = {1.0f, 0.0f, 0.0f, 1.0f}; /* identity, column-major */
    float B[4] = {1.0f, 2.0f, 3.0f, 4.0f}; /* column-major */
    float C[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int i;

    printf("  Test sgemm: ");
    cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 2, 2, 2,
                1.0f, A, 2, B, 2, 0.0f, C, 2);

    /* C should equal B since A is identity */
    for (i = 0; i < 4; i++) {
        if (!float_eq(C[i], B[i])) {
            printf("FAILED (C[%d]=%.2f, expected %.2f)\n", i, C[i], B[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}
#endif /* !SKIP_LEVEL3_TESTS */

/* Test CBLAS Level 1 Double: daxpy */
static int test_daxpy(void) {
    double x[N] = {1.0, 2.0, 3.0, 4.0};
    double y[N] = {1.0, 1.0, 1.0, 1.0};
    double alpha = 2.0;
    double expected[N] = {3.0, 5.0, 7.0, 9.0};
    int i;

    printf("  Test daxpy: ");
    cblas_daxpy(N, alpha, x, 1, y, 1);

    for (i = 0; i < N; i++) {
        if (fabs(y[i] - expected[i]) > EPSILON) {
            printf("FAILED (y[%d]=%.2f, expected %.2f)\n", i, y[i], expected[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}

#if !defined(SKIP_LEVEL3_TESTS)
/* Test CBLAS Level 3 Double: dgemm */
static int test_dgemm(void) {
    double A[4] = {1.0, 0.0, 0.0, 1.0}; /* identity, column-major */
    double B[4] = {1.0, 2.0, 3.0, 4.0}; /* column-major */
    double C[4] = {0.0, 0.0, 0.0, 0.0};
    int i;

    printf("  Test dgemm: ");
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, 2, 2, 2,
                1.0, A, 2, B, 2, 0.0, C, 2);

    for (i = 0; i < 4; i++) {
        if (fabs(C[i] - B[i]) > EPSILON) {
            printf("FAILED (C[%d]=%.2f, expected %.2f)\n", i, C[i], B[i]);
            return 1;
        }
    }
    printf("OK\n");
    return 0;
}
#endif /* !SKIP_LEVEL3_TESTS */

int main(int argc, char *argv[]) {
    int failures = 0;

    (void)argc;
    (void)argv;

    printf("OpenBLAS CBLAS functional tests for Nanvix\n");
    printf("==========================================\n\n");

    printf("CBLAS Level 1 (single precision):\n");
    failures += test_saxpy();
    failures += test_sdot();
    failures += test_snrm2();
    failures += test_sscal();
    failures += test_scopy();
    failures += test_isamax();

    printf("\nCBLAS Level 2 (single precision):\n");
    failures += test_sgemv();

#if !defined(SKIP_LEVEL3_TESTS)
    printf("\nCBLAS Level 3 (single precision):\n");
    failures += test_sgemm();
#else
    printf("\nCBLAS Level 3 (single precision): SKIPPED (P2/P5/P6 kernel)\n");
#endif

    printf("\nCBLAS Level 1 (double precision):\n");
    failures += test_daxpy();

#if !defined(SKIP_LEVEL3_TESTS)
    printf("\nCBLAS Level 3 (double precision):\n");
    failures += test_dgemm();
#else
    printf("\nCBLAS Level 3 (double precision): SKIPPED (P2/P5/P6 kernel)\n");
#endif

    printf("\n==========================================\n");
    if (failures == 0) {
        printf("All tests PASSED!\n");
        return 0;
    } else {
        printf("%d test(s) FAILED!\n", failures);
        return 1;
    }
}
