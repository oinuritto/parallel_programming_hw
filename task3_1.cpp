#include <mpi.h>
#include <cmath>
#include <iostream>

int rand(int min, int max) {
    return min + std::rand() % max;
}

const int N = 100;

int main(int argc, char** argv) {
    int rank, size;
    double localNorm = 0.0;
    double globalNorm = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int localSize = N / size;
    double globalVector[N];

    if (rank == 0) {
        printf("Vector: ");
        for (double & i : globalVector) {
            i = rand(0, 10);
            printf("%f ", i);
        }
        printf("\n");
    }

    double localVector[localSize];

    MPI_Scatter(globalVector, localSize, MPI_DOUBLE, localVector, localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < localSize; i++) {
        localNorm += std::fabs(localVector[i]);
    }

    MPI_Reduce(&localNorm, &globalNorm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        globalNorm = globalNorm;
        printf("Norm of the vector: %f\n", globalNorm);
    }

    MPI_Finalize();

    return 0;
}
