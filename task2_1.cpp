#include <iostream>
#include <mpi.h>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int* generateArray(int length) {
    int* arr = new int[length];
    for (int i = 0; i < length; ++i) {
        arr[i] = rand(0, 10);
    }
    return arr;
}

void printArray(int* arr, int length) {

    for (int i = 0; i < length; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

const int a = 5;
const int b = 6;
const int N = 10;

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int elementsPerProcess = N / (size - 1);
    int elementsForLastProcess = elementsPerProcess;

    if (N % (size - 1) > 0) {
        elementsPerProcess++;
        elementsForLastProcess = N % elementsPerProcess;
    }

    if (rank == 0) {
        int *x = generateArray(N);
        int *y = generateArray(N);
        int *z = new int[N];

        printf("a: %d\n", a);
        printf("b: %d\n", b);
        printf("x: ");
        printArray(x, N);
        printf("y: ");
        printArray(y, N);


        int startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                continue;
            int sizeToTransfer = elementsPerProcess;
            if (i == size - 1)
                sizeToTransfer = elementsForLastProcess;

            MPI_Send(&x[startIndex], sizeToTransfer, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&y[startIndex], sizeToTransfer, MPI_INT, i, 0, MPI_COMM_WORLD);

            startIndex += elementsPerProcess;
        }


        startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == 0)
                continue;

            int sizeToTransfer = elementsPerProcess;
            if (i == size - 1)
                sizeToTransfer = elementsForLastProcess;

            MPI_Recv(&z[startIndex], sizeToTransfer, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            startIndex += elementsPerProcess;
        }

        printf("z: ");
        printArray(z, N);
    } else {
        int count;
        MPI_Status status;

        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int *x = new int[count];
        int *y = new int[count];
        int *z = new int[count];

        MPI_Recv(x, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(y, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < count; i++) {
            z[i] = a * x[i] + b * y[i];
        }

        MPI_Send(z, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
