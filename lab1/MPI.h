#include <mpi.h>

struct TMPI {
    public:
        TMPI(int* argc = NULL, char*** argv = NULL) {
            MPI_Init(argc, argv);
        } 

        int getRank() {
            int rank = 0;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            return rank;
        }

        int getThreadsAmnt() {
            int amnt = 0;
            MPI_Comm_size(MPI_COMM_WORLD, &amnt);
            return amnt;
        }

        ~TMPI() {
            MPI_Finalize();
        }
};
