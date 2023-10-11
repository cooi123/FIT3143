#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "charging_node.h"
#include <pthread.h>
#define K 3
#define NODE_THRESHOLD 1
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISPLACEMENT 1

int shared_availability_counter = 0;
charging_node_logs *charging_logs;
pthread_mutex_t availabiltiy_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t charging_logs_mutex = PTHREAD_MUTEX_INITIALIZER;
int terminate_flag = 0;

int initialise_charging_grid(int size, int rank, int ndims, int *dims, MPI_Comm existing_comm, MPI_Comm *new_comm)
{

    int periods[ndims]; // Non-periodic grid
    int reorder = 0;    // preserve original ordering

    if (rank == 0)
        printf("Comm Size: %d: Grid Dimension =[%d x %d] \n", size, dims[0], dims[1]);

    int ierr = MPI_Cart_create(existing_comm, ndims, dims, periods, reorder, new_comm);
}
int incoming_car_simulator(int rank)
{
    srand(time(NULL) + rank);
    return rand() % 2;
}

void *thread_function(void *arg)
{
    while (!terminate_flag)
    {
        // Sleep for a random time between 0 and 1 second
        srand(time(NULL) + (int)arg);
        int sleep_time = rand() % 5;
        sleep(sleep_time);

        // Lock value mutex, perform operation, unlock
        pthread_mutex_lock(&availabiltiy_counter_mutex);
        int operation = rand() % 2;
        if (operation == 0)
        {
            shared_availability_counter++;
        }
        else if (operation == 1 && shared_availability_counter > 0)
        {
            shared_availability_counter--;
        }
        pthread_mutex_unlock(&availabiltiy_counter_mutex);
    }
    printf("terminating threads %d\n", (int)arg);
    return NULL;
}

void *communicate_neighbour_thread_func(void *arg)
{
    // int nodeRowUp, nodeRowBot, nodeColLeft, nodeColRight;
    // int coord[ndims];
    // MPI_Request neighbour_req[4];
    // MPI_Status neighbour_status[4];
    // MPI_Cart_coords(comm, rank, ndims, coord);

    // // get neighbour nodes
    // MPI_Cart_shift(comm, SHIFT_ROW, DISPLACEMENT, &nodeRowBot, &nodeRowUp);
    // MPI_Cart_shift(comm, SHIFT_COL, DISPLACEMENT, &nodeColLeft, &nodeColRight);
    // if (nodeRowUp != MPI_PROC_NULL)
    // {
    //     MPI_Isend(&val, 1, MPI_INT, nodeRowUp, 0, comm, &neighbour_req[0]);
    // }
    // if (nodeRowBot != MPI_PROC_NULL)
    // {
    //     MPI_Isend(&val, 1, MPI_INT, nodeRowBot, 0, comm, &neighbour_req[1]);
    // }
    // if (nodeColLeft != MPI_PROC_NULL)
    // {
    //     MPI_Isend(&val, 1, MPI_INT, nodeColLeft, 0, comm, &neighbour_req[2]);
    // }
    // if (nodeColRight != MPI_PROC_NULL)
    // {
    //     MPI_Isend(&val, 1, MPI_INT, nodeColRight, 0, comm, &neighbour_req[3]);
    // }
    // MPI_Waitall(4, &neighbour_req, &neighbour_status);
    printf("communicating\n");
    // printf("rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", rank, coord[0], coord[1], nodeColLeft, nodeColRight, nodeRowBot, nodeRowUp);
}

int charging_nodes_func(int size, int rank, int base, int ndims, MPI_Comm master_comm, MPI_Comm comm)
{
    MPI_Status status;
    MPI_Request req;

    int my_coords[ndims];
    const int k = 3;
    charging_logs = (charging_node_logs *)malloc(sizeof(charging_node_logs));
    charging_logs->size = 0;
    charging_logs->start_index = 0;
    charging_logs->end_index = -1;

    MPI_Cart_coords(comm, rank, ndims, my_coords);

    printf("Process %d has coordinates (%d, %d)\n", rank, my_coords[0], my_coords[1]);
    // send to base

    int arr[3] = {0};
    int sum = 0, iter = 100;
    int interval = 2, flag;
    pthread_t threads[K];
    pthread_t communication_thread;
    for (int i = 0; i < K; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, (void *)i);
    }
    int received_value;
    MPI_Irecv(&received_value, 1, MPI_INT, base, 0, master_comm, &req);

    while (1)
    {
        MPI_Test(&req, &flag, &status);
        if (received_value == TERMINATE_VALUE)
        {
            printf("Process %d received termination signal.\n", rank);
            terminate_flag = 1;
            for (int i = 0; i < K; i++)
            {
                pthread_join(threads[i], NULL);
            }
            break;
        }

        int val = incoming_car_simulator(rank);
        if (val <= NODE_THRESHOLD)
        {
            pthread_create(&communication_thread, NULL, communicate_neighbour_thread_func, (void *)rank);
        }

        pthread_mutex_lock(&charging_logs_mutex);
        insert_new_charging_node(charging_logs, val);
        printf("Printing logs from rank %d\n", rank);
        print_charging_logs(charging_logs);
        pthread_mutex_unlock(&charging_logs_mutex);

        sleep(interval);
        pthread_join(communication_thread, NULL);
    }
}

int insert_new_charging_node(charging_node_logs *logs, int availablity)
{
    charging_node node;
    get_time(&node.second, &node.minute, &node.hour, &node.day, &node.month, &node.year);
    node.avilablity = availablity;
    printf("%d/%d/%d %d:%d:%d, %d\n", node.day, node.month, node.year, node.hour, node.minute, node.second, node.avilablity);

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
    while (logs->start_index != logs->end_index)
    {
        printf("%d/%d/%d %d:%d:%d, %d\n", logs->logs[logs->start_index].day, logs->logs[logs->start_index].month, logs->logs[logs->start_index].year, logs->logs[logs->start_index].hour, logs->logs[logs->start_index].minute, logs->logs[logs->start_index].second, logs->logs[logs->start_index].avilablity);

        logs->start_index = (logs->start_index + 1) % 10;
    }
}