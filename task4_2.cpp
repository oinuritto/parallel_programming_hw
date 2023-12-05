#include <mpi.h>
#include <cstdio>
#include <cstdlib>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int arr[12];
    int block = 12 / size + 1;

    if (rank == 0) {
        for (int & i : arr) {
            i = rand(0, 10);
        }

        for (int i = 1; i < size; ++i) {
            int send_count = (i < 12 % size) ? block : block - 1;
            MPI_Send(arr + i * (block - 1), send_count, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int recv_count = (rank < 12 % size) ? block : block - 1;
        MPI_Recv(arr, recv_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("Process %d: ", rank);
    int count = (rank < 12 % size) ? block : block - 1;

    for (int i = 0; i < count; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    MPI_Finalize();
    return 0;
}
