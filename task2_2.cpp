#include <iostream>
#include <mpi.h>


int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char **argv) {
    const int N = 12;

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int part_size = N / (size - 1);
    int lastPart_size = part_size;

    if (N % (size - 1) > 0) {
        part_size++;
        lastPart_size = N % part_size;
    }


    if (rank == 0) {
        int vector[N];
        int matrix[N][N];
        int result[N];

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

        int startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                continue;
            int sizeToTransfer = part_size;
            if (i == size - 1)
                sizeToTransfer = lastPart_size;

            MPI_Send(&matrix[startIndex][0], sizeToTransfer * N, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&vector[0], N, MPI_INT, i, 0, MPI_COMM_WORLD);

            startIndex += part_size;
        }

        startIndex = 0;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&result[startIndex], part_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            startIndex += part_size;
        }

        printf("Result vector: \n");
        for (int i: result) {
            printf("%d ", i);
        }
        printf("\n");
    } else {
        int localVector[N];
        int localMatrix[part_size][N];
        int localResult[N];

        int count;

        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        MPI_Recv(&localMatrix[0][0], count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&localVector[0], N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < part_size; ++i) {
            localResult[i] = 0;
            for (int j = 0; j < N; ++j) {
                localResult[i] += localMatrix[i][j] * localVector[j];
            }
        }

        MPI_Send(&localResult[0], part_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}