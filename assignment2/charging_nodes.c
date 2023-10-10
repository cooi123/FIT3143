#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "charging_node.h"

int initialise_charging_grid(int size, int rank, int ndims, int *dims, MPI_Comm existing_comm, MPI_Comm *new_comm)
{

    int periods[ndims]; // Non-periodic grid
    int reorder = 0;    // preserve original ordering

    if (rank == 0)
        printf("Comm Size: %d: Grid Dimension =[%d x %d] \n", size, dims[0], dims[1]);

    int ierr = MPI_Cart_create(existing_comm, ndims, dims, periods, reorder, new_comm);
}
int incoming_car_simulator()
{
    return rand() % 2;
}

int charging_nodes_func(int size, int rank, int base, int ndims, MPI_Comm master_comm, MPI_Comm comm)
{
    MPI_Status status;
    MPI_Request req;

    int my_coords[ndims];
    const int k = 3;
    charging_node_logs charging_logs = {.size = 0, .start_index = 0, .end_index = -1};

    MPI_Cart_coords(comm, rank, ndims, my_coords);

    printf("Process %d has coordinates (%d, %d)\n", rank, my_coords[0], my_coords[1]);
    // send to base

    int arr[3] = {0};
    int sum = 0, iter = 100;
    int interval = 5;
    time_t now;
    // int runtime = now;
    for (int i = 0; i < iter; i++)
    {
        now = time(NULL);
        if (now % interval == 0)
        {

            for (int i = 0; i < k; i++)
            {
                int val = incoming_car_simulator();
                arr[i] = val;
                sum += arr[i];
            }
            insert_new_charging_node(&charging_logs, sum);
            MPI_Isend(&sum, 1, MPI_INT, base, 0, master_comm, &req);
            sum = 0;
            // print_charging_logs(&charging_logs);
        }
    }
}

int insert_new_charging_node(charging_node_logs *logs, int availablity)
{
    charging_node node;
    get_time(&node.second, &node.minute, &node.hour, &node.day, &node.month, &node.year);
    node.avilablity = availablity;
    if (logs->size == 10)
    {
        logs->start_index = (logs->start_index + 1) % 10;
        logs->end_index = (logs->end_index + 1) % 10;
        logs->logs[logs->end_index] = node;
    }
    else
    {
        logs->end_index = (logs->end_index + 1) % 10;
        logs->logs[logs->end_index] = node;
        logs->size++;
    }
    return 0;
}

int get_time(int *second, int *minute, int *hour, int *day, int *month, int *year)
{
    time_t now = time(NULL);
    struct tm *ptm = gmtime(&now);
    *second = ptm->tm_sec;
    *minute = ptm->tm_min;
    *hour = ptm->tm_hour;
    *day = ptm->tm_mday;
    *month = ptm->tm_mon;
    *year = ptm->tm_year;
    return 0;
}

void print_charging_logs(charging_node_logs *logs)
{
    for (int i = 0; i < logs->size; i++)
    {
        printf("Node %d: %d/%d/%d %d:%d:%d, %d\n", i, logs->logs[i].day, logs->logs[i].month, logs->logs[i].year, logs->logs[i].hour, logs->logs[i].minute, logs->logs[i].second, logs->logs[i].avilablity);
    }
}