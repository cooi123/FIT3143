#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "charging_node.h"
#include "base.h"
#include "shared.h"
#define NDIMS 2

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
        base_func(base, MPI_COMM_WORLD, dims, size - 1);
    }
    else
    {
        initialise_charging_grid(size - 1, rank, NDIMS, dims, node_comm, &node_grid_comm);
        charging_nodes_func(size, rank, base, NDIMS, MPI_COMM_WORLD, node_grid_comm);
    }
    MPI_Finalize();
    return 0;
}
