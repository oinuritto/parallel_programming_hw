#include <mpi.h>
#include <cstdio>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int number = rank * 10;

    if (rank == 0) {
        int* numbers = new int[size];
        MPI_Status status;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&numbers[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }

        for (int i = 1; i < size; i++) {
            int min_rank = i;
            for (int j = i + 1; j < size; j++) {
                if (numbers[j] < numbers[min_rank]) {
                    min_rank = j;
                }
            }
            int temp = numbers[i];
            numbers[i] = numbers[min_rank];
            numbers[min_rank] = temp;
            printf("%d ", numbers[i]);
        }
        printf("\n");

        delete[] numbers;
    }
    else {
        MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}