#include <mpi.h>
#include <random>
#include <ctime>

int main(int argc, char** argv) {
    int rank, count, size;

    const int LENGTH = 10;
    int a[LENGTH];

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 1) {
        // Создаем случайный массив на процессе 0
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        for (int & i : a) {
            i = std::rand() % 100; // Заполняем случайными числами от 0 до 99
        }

        // отправка другим процессам
        for (int i = 0; i < size; i++) {
            if (i != 1) {
                MPI_Send(&a, 10, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Probe(1, 1, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        MPI_Recv(&a, count, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);

        for (int i : a) {
            printf("%d ", i);
        }
        printf("\n");
    }

    MPI_Finalize();
}
