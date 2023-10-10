#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define TERMINATE_VALUE -1

int main(int argc, char **argv)
{

    int rank;
    int world_size;
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request req;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (rank == 0)
    {
        printf("%d running\n", rank);
        int recbuf;
        int counter_0 = 0;
        while (counter_0 < world_size)
        {
            MPI_Recv(&recbuf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            counter_0++;
            printf("recived %d from %d, current counter val%d", recbuf, status.MPI_SOURCE, counter_0);
        }
    }
    else
    {
        printf("%d is still running\n", rank);
        int counter = 0;
        int val = 100;
        MPI_Isend(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);
        while (counter < 1000)
        {
            counter++;
        }
    }
    MPI_Finalize();
    return 0;
}
