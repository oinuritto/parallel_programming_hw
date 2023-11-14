#include <cstdio>
#include <mpi.h>
#include <iostream>

struct MaxInfo {
    double value;
    int rank;
};

int rand(int min, int max) {
    return min + std::rand() % max;
}

const int VECTOR_SIZE = 100;

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double vector[VECTOR_SIZE];

    if (rank == 0) {
        printf("Vector: \n");
        for (double &i : vector) {
            i = rand(0, 50) * 1.02;
            printf("%.2f ", i);
        }
        printf("\n");
    }

    int localSize = VECTOR_SIZE / size;

    double localVector[localSize];

    MPI_Scatter(vector, localSize, MPI_DOUBLE, localVector, localSize, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MaxInfo localMax = {0.0, 0};

    for (int i = 0; i < localSize; ++i) {
        if (localVector[i] > localMax.value) {
            localMax.value = localVector[i];
            localMax.rank = rank;
        }
    }

    MaxInfo result_all[localSize];
    MPI_Gather(&localMax, sizeof(MaxInfo), MPI_BYTE, result_all, sizeof(MaxInfo), MPI_BYTE, 0, MPI_COMM_WORLD);

    MaxInfo globalMax{};

    MPI_Reduce(&localMax, &globalMax, 1, MPI_DOUBLE_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < localSize; i++) {
            printf("Local maximum value = %.2f. Rank = %d\n", result_all[i].value, result_all[i].rank);
        }
        printf("Global maximum value = %.2f, Rank = %d\n", globalMax.value, globalMax.rank);
    }

    MPI_Finalize();
    return 0;
}
