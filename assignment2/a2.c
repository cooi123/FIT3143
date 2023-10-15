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
    int rank, size, nrows, ncols, provided;
    int dims[NDIMS] = {0}, coord[NDIMS];

    MPI_Comm node_comm, node_grid_comm;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int base = size - 1;

    MPI_Comm_split(MPI_COMM_WORLD, rank == base, 0, &node_comm);

    char *dirname;
    dirname = "output_logs";
    if (argc == 3)
    {
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        dims[0] = nrows;
        dims[1] = ncols;
        if ((nrows * ncols) != size - 1)
        {
            if (rank == base)
                printf("ERROR: nrows*ncols)=%d *%d = %d != %d\n", nrows, ncols, nrows * ncols, size);
            MPI_Finalize();
            return 0;
        }
    }
    else if (argc > 1)
    {
        dirname = argv[1];
    }

    // if not specify using mpi to assign.
    MPI_Dims_create(size - 1, NDIMS, dims);
    double start_time, end_time, total_time, max_time;
    start_time = MPI_Wtime();
    if (rank == base)
    {
        base_func(base, MPI_COMM_WORLD, dims, size - 1, dirname);
    }
    else
    {
        initialise_charging_grid(size - 1, rank, NDIMS, dims, node_comm, &node_grid_comm, dirname);
        charging_nodes_func(size, rank, base, NDIMS, MPI_COMM_WORLD, node_grid_comm, dirname);
        MPI_Comm_free(&node_grid_comm);
        MPI_Comm_free(&node_comm);
    }
    end_time = MPI_Wtime();
    total_time = end_time - start_time;
    MPI_Reduce(&total_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, base, MPI_COMM_WORLD);

    if (rank == base)
    {
        char filename[200];
        FILE *file;
        snprintf(filename, sizeof(filename), "%s/base_node_log.txt", dirname);

        file = fopen(filename, "a");
        fprintf(file, "Total time taken: %f\n", total_time);
        fprintf(file, "Max time taken: %f\n", max_time);
    }
    MPI_Finalize();
    return 0;
}
