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

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0)
    {
        printf("%d running\n", rank);
        sleep(10);
        int term = TERMINATE_VALUE;
        printf("Terminate")
            MPI_Bcast(&term, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        printf("%d is still running\n", rank);

        while (1)
        {
            int received_value;
            MPI_Bcast(&received_value, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (received_value == TERMINATE_VALUE)
            {
                printf("Process %d received termination signal.\n", rank);
                break;
            }
        }
    }
    MPI_Finalize();
    return 0;
}
