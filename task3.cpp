#include <iostream>
#include <ctime>
#include <cstdlib>
#include <mpi.h>

int main(int argc, char** argv)
{
    int rank;
    int const SIZE = 10;
    int a[SIZE];

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        // Создаем случайный массив на процессе 0
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        for (int & i : a) {
            i = std::rand() % 100; // Заполняем случайными числами от 0 до 99
        }

        MPI_Send(a, SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        MPI_Recv(a, SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        for (int i : a) {
            printf("%d ", i);
        }
    }

    MPI_Finalize();
}
