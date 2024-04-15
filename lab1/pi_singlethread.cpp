#include "MPI.h"
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "Enter steps amount\n";
        return EXIT_FAILURE;
    }
    TMPI mpi;

    unsigned stepsAmnt = std::atoi(argv[1]);
    int threadsAmnt = mpi.getThreadsAmnt();
    double step = 1./stepsAmnt;
    long long stepsPerThread = stepsAmnt / threadsAmnt;

    int rank = mpi.getRank();    
    
    int leftBorder  = rank * stepsPerThread;
    int rightBorder = (rank + 1) * stepsPerThread;

    double res = 0;
    for (int i = leftBorder; i < rightBorder; ++i) {
        double x = (i + 0.5) * step;
        res += 4.0 / (1 + x * x) * step;
        // sleep(1); // to get a clearer result
    }

    if (rank == 0) {
        for (int i = 1; i < threadsAmnt; ++i) {
            double tmp = 0;
            MPI_Recv(&tmp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            res += tmp;
        }

        std::cout << "Pi = " << std::setprecision(20) << res << std::endl;
    }
    else {
        MPI_Send(&res, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    return EXIT_SUCCESS;
}
