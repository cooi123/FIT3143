#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DIMS_N 2
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISPLAY 1

int main(int argc, char *argv[])
{

    int m = 3, n = 3, myRank, size, reorder, cartRank;
    int dims[DIMS_N], coord[DIMS_N], periods[DIMS_N];
    MPI_Comm chargingNode;
    int nodeRowUp, nodeRowBot, nodeColLeft, nodeColRight;
    ;

    /*start up MPI environment*/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    dims[0] = m;
    dims[1] = n;

    reorder = 1;
    periods[0] = periods[1] = 0;

    /*Create Cartesian grid layout*/
    MPI_Cart_create(MPI_COMM_WORLD, DIMS_N, dims, periods, reorder, &chargingNode);

    /*Get the coordinates*/
    MPI_Cart_coords(chargingNode, myRank, DIMS_N, coord);

    /*Get the rank based on the corrdinates*/
    MPI_Cart_rank(chargingNode, coord, &cartRank);

    /*Get neighbour node*/
    MPI_Cart_shift(chargingNode, SHIFT_ROW, DISPLACEMENT, &nodeRowBot, &nodeRowUp);
    MPI_Cart_shift(chargingNode, SHIFT_COL, DISPLACEMENT, &nodeColLeft, &nodeColRight);

    printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", myRank, cartRank, coord[0], coord[1], nodeColLeft, nodeColRight, nodeRowBot, nodeRowUp);

    fflush(stdout);
    MPI_Comm_free(&chargingNode);
    MPI_Finalize();
    return 0;
}