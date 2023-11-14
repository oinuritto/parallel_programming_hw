#include <iostream>
#include <mpi.h>


int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char **argv) {
    const int N = 12;

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int rowsPerProcess = N / size;
    const int elementsPerProcess = rowsPerProcess * N;

    int vector[N];
    int matrix[N][N];

    if (rank == 0) {
        printf("Matrix: \n");
        for (int i = 0; i < N; ++i) {
            vector[i] = rand(0, 10);
            for (int j = 0; j < N; ++j) {
                matrix[i][j] = rand(0, 10);
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        printf("Vector: \n");
        for (int i: vector) {
            printf("%d ", i);
        }
        printf("\n");

        for (int i = 1; i < size; ++i) {
            int startRow = i * rowsPerProcess;
            MPI_Send(vector, N, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&matrix[startRow][0], elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(vector, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(matrix, elementsPerProcess, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int buf[N];

    for (int i = 0; i < N; ++i) {
        buf[i] = 0;
        for (int k = 0; k < N; ++k) {
            buf[i] += matrix[i][k] * vector[k];
        }
    }

    if (rank == 0) {
        int result[N];

        for (int i = 1; i < size; ++i) {
            MPI_Recv(result, N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < N; ++j) {
                result[j] = buf[j];
            }
        }

        printf("Result vector: \n");
        for (int i: result) {
            printf("%d ", i);
        }
        printf("\n");
    } else {
        MPI_Send(buf, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}