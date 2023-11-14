#include <iostream>
#include <mpi.h>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 12;
    const int elementsPerProcess = (N * N) / size;

    int a[N][N];
    int b[N][N];
    int localA[N][N];
    int localB[N][N];
    int localC[N][N];

    if (rank == 0) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                a[i][j] = rand(0, 10);
                b[i][j] = rand(0, 10);
            }
        }

        printf("Matrix A:\n");
        for (auto & i : a) {
            for (int j : i) {
                printf("%d ", j);
            }
            printf("\n");
        }

        printf("Matrix B:\n");
        for (auto & i : b) {
            for (int j : i) {
                printf("%d ", j);
            }
            printf("\n");
        }

        for (int i = 1; i < size; ++i) {
            int start = i * elementsPerProcess;
            int end = (i == size - 1) ? (N * N) : ((i + 1) * elementsPerProcess);
            MPI_Send(&a[0][0] + start, end - start, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&b[0][0] + start, end - start, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                localA[i][j] = a[i][j];
                localB[i][j] = b[i][j];
            }
        }
    } else {
        MPI_Recv(&localA[0][0], elementsPerProcess, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&localB[0][0], elementsPerProcess, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            localC[i][j] = localA[i][j] * localB[i][j];
        }
    }

    if (rank == 0) {
        int resultMatrix[N][N];

        for (int i = 1; i < size; ++i) {
            int start = i * elementsPerProcess;
            int end = (i == size - 1) ? (N * N) : ((i + 1) * elementsPerProcess);
            MPI_Recv(&resultMatrix[0][0] + start, end - start, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                resultMatrix[i][j] = localC[i][j];
            }
        }

        printf("Matrix C:\n");
        for (auto & i : resultMatrix) {
            for (int j : i) {
                printf("%d ", j);
            }
            printf("\n");
        }
    } else {
        MPI_Send(&localC[0][0], elementsPerProcess, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}