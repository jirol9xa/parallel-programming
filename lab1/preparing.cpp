#include <mpi.h>
#include <iostream>
#include <thread>
#include <utility>
#include <chrono>

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    int buff = 0;
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        auto start = std::chrono::high_resolution_clock::now();
        MPI_Send(&buff, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&buff, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        auto end = std::chrono::high_resolution_clock::now(); 

        std::cout << (end - start) / 2 << std::endl;
    }
    else if (rank == 1) {
        MPI_Recv(&buff, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&buff, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return 0;
}