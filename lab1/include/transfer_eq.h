#pragma once

#include <cstdint>
#include <inttypes.h>
#include <ios>
#include <vector>
#include <functional>
#include <fstream>

namespace NTransferEq {
    using ui32 = std::uint32_t;

    class TColumnProcessor {
    private:
        const int MpiRank = 0;

        const ui32 Size = 0;

        const double TimeStep = 0;
        const double XStep = 0;

        std::vector<double> OldRow;
        std::vector<double> NewRow;

        std::function<double(double)> GetXInitValue;    // Time = 0, X = ?
        std::function<double(double, bool isXMax)> GetTInitValue;    // Time = ?, X = 0 || X_MAX

        //std::ofstream DumpFile{"dump.txt", std::ios_base::app};

    private:
        void Dump();

    public:
        TColumnProcessor(int rank, ui32 size, double timeStep, double xStep, std::function<double(double)> xInit, std::function<double(double, bool)> tInit) : 
            MpiRank(rank), Size(size), TimeStep(timeStep), XStep(xStep), OldRow(Size), NewRow(Size), GetXInitValue(xInit), GetTInitValue(tInit)
        {}

        void ProcessRow(double currentTime);
        void FinishRow(double currentTime);
    };
}
