#include <cstdio>
#include <mpi.h>
#include <iostream>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char **argv) {
    const int N = 12;

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int matrix[N][N];
    int transposedMatrix[N][N];

    if (rank == 0) {
        printf("Matrix: \n");
        for (auto & i : matrix) {
            for (int & j : i) {
                j = rand(0, 10);
                printf("%d ", j);
            }
            printf("\n");
        }

        for (int i = 1; i < size; i++) {
            for (int row = i - 1; row < N; row += size - 1) {
                MPI_Send(matrix[row], N, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        for (int row = rank - 1; row < N; row += size - 1) {
            MPI_Recv(matrix[row], N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            transposedMatrix[j][i] = matrix[i][j];
        }
    }

    if (rank == 0) {
        int result[N][N];

        for (int i = 1; i < size; i++) {
            MPI_Recv(&result[0][0], N * N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    if (result[j][k] == 0) {
                        result[j][k] = transposedMatrix[j][k];
                    }
                }
            }
        }
        printf("Transposed matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", transposedMatrix[i][j]);
            }
            printf("\n");
        }
    } else {
        MPI_Send(&transposedMatrix[0][0], N * N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}