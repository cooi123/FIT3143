#include <mpi.h>
int base_func(int base, MPI_Comm master_comm, int *dims, int num_chraging_nodes);
void rank_to_coords(int rank, int *dims, int *coords);
int get_cart_neighbors(int rank, int *dims, int *neighbors);