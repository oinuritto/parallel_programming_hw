#include <cstdio>
#include <mpi.h>

const int N = 100;

int main(int argc, char** argv) {
    int rank, size;
    double localDotProduct = 0.0;
    double globalDotProduct = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int localSize = N / size;

    double globalVector1[N];
    double globalVector2[N];

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            globalVector1[i] = i / 3.0;
            globalVector2[i] = i * 4.0 / 3.0;
        }
        printf("Vector 1:\n");
        for (double i : globalVector1) {
            printf("%.2f ", i);
        }
        printf("\n");

        printf("Vector 2:\n");
        for (double i : globalVector2) {
            printf("%.2f ", i);
        }
        printf("\n");
    }

    double localVector1[localSize];
    double localVector2[localSize];

    MPI_Scatter(globalVector1, localSize, MPI_DOUBLE, localVector1, localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(globalVector2, localSize, MPI_DOUBLE, localVector2, localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < localSize; i++) {
        localDotProduct += localVector1[i] * localVector2[i];
    }

    MPI_Reduce(&localDotProduct, &globalDotProduct, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result: %.2f\n", globalDotProduct);
    }

    MPI_Finalize();

    return 0;
}

