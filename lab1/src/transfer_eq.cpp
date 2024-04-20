#include "transfer_eq.h"
#include "mpi.h"
#include "mpi_common.h"

#include <algorithm>
#include <fstream>
#include <inttypes.h>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <iostream>

using namespace NTransferEq;

void TColumnProcessor::ProcessRow(double currentTime) {
    if (std::fabs(currentTime - 0) < 1e-9) {
        double x = 0;
        for (int i = 0; i < Size; ++i) {
            NewRow[i] = GetXInitValue(x);
            x += XStep;
        }
        OldRow = NewRow;
        return;
    } 
    
    for (int i = 1; i < Size - 1; ++i) {
        double u1 = OldRow[i - 1]; // u1 -> U_{x - 1}
        double u2 = OldRow[i]; // u2 -> U_{x}
        double u3 = OldRow[i + 1]; // u3 -> U_{x + 1}
    
        NewRow[i] = u2 - TimeStep / XStep * (u3 - u1); // u3? T * F?
    }
}

void TColumnProcessor::FinishRow(double currentTime) {
    int maxRank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &maxRank);

    if (MpiRank != 0) {
        MPI_Send(&OldRow[0], 1, MPI_DOUBLE, MpiRank - 1, 0, MPI_COMM_WORLD);
        double mostLeft = 0;
        MPI_Recv(&mostLeft, 1, MPI_DOUBLE, MpiRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        NewRow.front() = OldRow[0] - TimeStep / XStep * (OldRow[0 + 1] - mostLeft);
    }
    else {
        int dummy;
        // ensure that last thread finish dump
        MPI_Recv(&dummy, 1, MPI_INT, maxRank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        NewRow.front() = GetTInitValue(currentTime, false);
    }
    
    if (MpiRank != maxRank - 1) {
        double mostRight = 0;
        MPI_Recv(&mostRight, 1, MPI_DOUBLE, MpiRank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        NewRow.back() = OldRow.back() - TimeStep / XStep * (mostRight - *(OldRow.rbegin() + 1));

        // Place dump func here cause of avoiding race over the dump file
        Dump();

        MPI_Send(&OldRow.back(), 1, MPI_DOUBLE, MpiRank + 1, 0, MPI_COMM_WORLD);
    }
    else {
        NewRow.back() = GetTInitValue(currentTime, true);
        Dump();
    }

    NewRow.swap(OldRow);
}

void TColumnProcessor::Dump() {
    std::stringstream ss;
    for (auto elem : NewRow) {
        ss << std::showpos << MpiRank << "**" << std::showpos << std::setw(10) << std::fixed << std::setprecision(2) << elem << ' ';
    }
    std::ofstream DumpFile("dump.txt", std::ios_base::app);
    DumpFile << ss.str();
}
