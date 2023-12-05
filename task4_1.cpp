#include <iostream>
#include <mpi.h>

const int N = 50001;

int rand(int min, int max) {
    return min + std::rand() % max;
}

void ping_pong(int rank, int msg_size);

void ping_ping(int rank, int msg_size);

int main(int argc, char **argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2) {
        if (rank == 0) {
            printf("This program requires exactly 2 processes.");
        }
        MPI_Finalize();
        return 1;
    }

    for (int msg_size = 1; msg_size <= N; msg_size += 10000) {
        ping_pong(rank, msg_size);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Ждем завершения ping_pong для всех процессов

    for (int msg_size = 1; msg_size <= N; msg_size += 10000) {
        ping_ping(rank, msg_size);
    }

    MPI_Finalize();
    return 0;
}

void ping_pong(int rank, int msg_size) {
    int *msg = new int[msg_size];

    for (int i = 0; i < msg_size; ++i) {
        msg[i] = rand(1, 100);
    }

    if (rank == 0) {
        auto start = MPI_Wtime();
        MPI_Send(msg, msg_size, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(msg, msg_size, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        auto end = MPI_Wtime();

        double duration = end - start;
        printf("Ping-Pong. Time for msg_size %d: %.6f seconds\n", msg_size, duration);
    } else {
        int *received = new int[msg_size];
        MPI_Recv(received, msg_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(received, msg_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    delete[] msg;
}


void ping_ping(int rank, int msg_size) {
    int *msg = new int[msg_size];

    for (int i = 0; i < msg_size; ++i) {
        msg[i] = rand(0, 100);
    }

    MPI_Request send_request, recv_request;

    auto start = MPI_Wtime();

    if (rank == 0) {
        MPI_Isend(msg, msg_size, MPI_INT, 1, 1, MPI_COMM_WORLD, &send_request);
        MPI_Irecv(msg, msg_size, MPI_INT, 1, 1, MPI_COMM_WORLD, &recv_request);
    } else {
        MPI_Isend(msg, msg_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &send_request);
        MPI_Irecv(msg, msg_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &recv_request);
    }

    MPI_Wait(&send_request, MPI_STATUS_IGNORE);
    MPI_Wait(&recv_request, MPI_STATUS_IGNORE);

    auto end = MPI_Wtime();

    double duration = end - start;
    if (rank == 0) {
        printf("Ping-Ping. Time for msg_size %d: %.6f seconds\n", msg_size, duration);
    }

    delete[] msg;
}

