#include <mpi.h>
#include <cstdio>
#include <iostream>

int rand(int min, int max) {
    return min + std::rand() % max;
}


const int MATRIX_SIZE = 100;
const int VECTOR_SIZE = 100;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double matrix[MATRIX_SIZE][MATRIX_SIZE];
    double vector[VECTOR_SIZE];
    double local_result[MATRIX_SIZE];

    if (rank == 0) {
//        printf("Matrix: \N");
        for (auto & i : matrix) {
            for (double & j : i) {
                j = rand(0, 10);
//                printf("%.2f ", j);
            }
//            printf("\N");
        }

//        printf("Vector:\N");
        for (double & i : vector) {
            i = rand(0, 10);
//            printf("%.2f ", i);
        }
//        printf("\N");
    }

    MPI_Bcast(vector, VECTOR_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int localRows = MATRIX_SIZE / size;
    double local_matrix[localRows][MATRIX_SIZE];

    MPI_Scatter(matrix, localRows * MATRIX_SIZE, MPI_DOUBLE, local_matrix, localRows * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < localRows; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            local_result[i] += local_matrix[i][j] * vector[j];
        }
    }

    double global_result[MATRIX_SIZE];
    MPI_Gather(local_result, localRows, MPI_DOUBLE, global_result, localRows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result:\n");
        for (double i : global_result) {
            printf("%f ", i);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
