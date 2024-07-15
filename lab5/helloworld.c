#include <stdio.h>
#include "mpi.h"

int main(int arc, char *argv[])
{
    int rank, size;
    MPI_Init(&arc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int val = -1;
    do
    {
        if (rank == 0)
        {
            printf("Enter a round number (> 0 ): ");
            fflush(stdout);
            scanf("%d", &val);
        }
        MPI_Bcast(&val, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("Processors: %d. Received Value: %d\n", rank, val);
        fflush(stdout);
    } while (val > 0);

    MPI_Finalize();
    return 0;
}