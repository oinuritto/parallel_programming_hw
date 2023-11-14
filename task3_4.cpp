#include <mpi.h>
#include <cstdio>
#include <iostream>

int rand(int min, int max) {
    return min + std::rand() % max;
}

const int MATRIX_SIZE = 100;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double matrix[MATRIX_SIZE][MATRIX_SIZE];
    double local_max = 0;

    if (rank == 0) {
        for (auto & i : matrix) {
            for (auto & j : i) {
                j = rand(0, 10) * 1.02;
            }
        }
    }

    int localRows = MATRIX_SIZE / size;
    double local_matrix[localRows][MATRIX_SIZE];

    MPI_Scatter(matrix, localRows * MATRIX_SIZE, MPI_DOUBLE, local_matrix, localRows * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < localRows; i++) {
        double row_sum = 0;
        for (int j = 0; j < MATRIX_SIZE; j++) {
            row_sum += std::abs(local_matrix[i][j]);
        }
        if (row_sum > local_max) {
            local_max = row_sum;
        }
    }

    double global_max = 0.0;
    MPI_Reduce(&local_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Norm of the matrix: %.2f\n", global_max);
    }

    MPI_Finalize();
    return 0;
}
