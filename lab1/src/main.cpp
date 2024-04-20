#include "mpi_common.h"
#include "transfer_eq.h"

#include <fstream>
#include <ios>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace NTransferEq;

int main(int argc, const char* const argv[]) {
    if (argc < 4) {
        std::cout << "Run format \"./prog <x_step> <time_step> <max_time>\"";
        return EXIT_FAILURE;
    }

    TMPI mpi;
    
    int threadsAmnt = mpi.GetThreadsAmnt();
    int rank = mpi.GetRank();
    double xStep = std::stod(argv[1]);
    double timeStep = std::stod(argv[2]);
    double maxTime = std::stod(argv[3]);
    ui32 xAxisSize = 1 / xStep;

    TColumnProcessor proc(rank, xAxisSize / threadsAmnt, timeStep, xStep, [](double arg){ return sin(arg); }, [](double arg, bool){ return 0; });
    if (rank == threadsAmnt - 1) {
        int dummy;
        MPI_Send(&dummy, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    
    double curTime = 0;
    for (int i = 0; i < int(maxTime / timeStep); ++i, curTime += timeStep) {
        proc.ProcessRow(curTime);
        proc.FinishRow(curTime);
        
        if (rank == threadsAmnt - 1 && i != int(maxTime / timeStep) - 1) {
            std::ofstream dump{"dump.txt", std::ios_base::app};
            dump << std::endl;
            int dummy;
            // signal the first thread, that dump finished
            MPI_Send(&dummy, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    
    if (rank == threadsAmnt - 1) {
        std::ofstream dump{"dump.txt", std::ios_base::app};
        dump << std::endl << std::endl;
    }

    return 0;
}
