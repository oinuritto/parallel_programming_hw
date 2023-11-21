#include <iostream>
#include <mpi.h>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 12;
    int part_size = N / (size - 1);
    int lastPart_size = part_size;

    if (N % (size - 1) > 0) {
        part_size++;
        lastPart_size = N % part_size;
    }

    if (rank == 0) {
        int a[N][N];
        int b[N][N];
        int c[N][N];

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

        int startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                continue;
            int sizeToTransfer = part_size;
            if (i == size - 1)
                sizeToTransfer = lastPart_size;

            MPI_Send(&a[startIndex][0], sizeToTransfer * N, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&b[0][0], N * N, MPI_INT, i, 0, MPI_COMM_WORLD);

            startIndex += part_size;
        }

        startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                continue;

            int sizeToTransfer = part_size;
            if (i == size - 1)
                sizeToTransfer = lastPart_size;

            MPI_Recv(&c[startIndex][0], sizeToTransfer * N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            startIndex += part_size;
        }

        printf("Matrix C:\n");
        for (auto & i : c) {
            for (int j : i) {
                printf("%d ", j);
            }
            printf("\n");
        }

    } else {
        int localA[N][N];
        int localB[N][N];
        int localC[N][N];

        int count;

        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        MPI_Recv(&localA[0][0], count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&localB[0][0], N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < part_size; ++i) {
            for (int j = 0; j < N; ++j) {
                localC[i][j] = 0;
                for (int k = 0; k < N; ++k) {
                    localC[i][j] += localA[i][k] * localB[k][j];
                }
            }
        }

        MPI_Send(&localC[0][0], count, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}