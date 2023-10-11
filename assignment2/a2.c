#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>
#include "charging_node.h"
#define NDIMS 2
int base_func(int base, MPI_Comm master_comm, MPI_Comm comm);

int main(int argc, char **argv)
{
    int rank, size, nrows, ncols;
    int dims[NDIMS] = {0}, coord[NDIMS];

    MPI_Comm node_comm, node_grid_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int base = size - 1;

    MPI_Comm_split(MPI_COMM_WORLD, rank == base, 0, &node_comm);

    if (argc == 3)
    {
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        if ((nrows * ncols) != size - 1)
        {
            if (rank == base)
                printf("ERROR: nrows*ncols)=%d *%d = %d != %d\n", nrows, ncols, nrows * ncols, size);
            MPI_Finalize();
            return 0;
        }
    }
    else
    {
        // if not specify using mpi to assign.
        MPI_Dims_create(size - 1, NDIMS, dims);
    }

    if (rank == base)
    {
        base_func(base, MPI_COMM_WORLD, &node_comm);
    }
    else
    {
        initialise_charging_grid(size - 1, rank, NDIMS, dims, node_comm, &node_grid_comm);
        charging_nodes_func(size, rank, base, NDIMS, MPI_COMM_WORLD, node_grid_comm);
    }
    MPI_Finalize();
    return 0;
}

int base_func(int base, MPI_Comm master_comm, MPI_Comm comm)
{
    int recbuf;
    MPI_Status status;
    int iter = 100;
    for (int i = 0; i < iter; i++)
    {
        // MPI_Irecv(&recbuf, 1, MPI_INT, MPI_ANY_SOURCE, 0, master_comm, &status);
        // printf("recived %d from %d", recbuf, status.MPI_SOURCE);
    }
    sleep(5);
    int term = TERMINATE_VALUE;
    printf("Terminate");
    for (int node = 0; node < base; node++)
    {
        MPI_Send(&term, 1, MPI_INT, node, 0, master_comm);
    }
    return 0;
}