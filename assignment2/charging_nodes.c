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
#define MAX_NEIGHBOURS 4

int shared_availability_counter = 0;
charging_node_logs *charging_logs;
pthread_mutex_t availabiltiy_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t charging_logs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t termiate_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int terminate_flag = 0;
volatile int communication_thread_done = 0;
int shared_ndims;
int shared_rank;
MPI_Comm shared_comm;

int initialise_charging_grid(int size, int rank, int ndims, int *dims, MPI_Comm existing_comm, MPI_Comm *new_comm)
{

    int periods[2] = {0, 0}; // Non-periodic grid
    int reorder = 0;         // preserve original ordering

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
        if (operation == 0 && shared_availability_counter < K)
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
    int nodeRowUp, nodeRowBot, nodeColLeft, nodeColRight;
    int coord[shared_ndims];
    MPI_Cart_coords(shared_comm, shared_rank, shared_ndims, coord);

    int neighbours = 0;
    int temp_val = 1;

    // get neighbour nodes
    MPI_Cart_shift(shared_comm, SHIFT_ROW, DISPLACEMENT, &nodeRowBot, &nodeRowUp);
    MPI_Cart_shift(shared_comm, SHIFT_COL, DISPLACEMENT, &nodeColLeft, &nodeColRight);
    printf("rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", shared_rank, coord[0], coord[1], nodeColLeft, nodeColRight, nodeRowBot, nodeRowUp);

    int all_neighbour_ranks[MAX_NEIGHBOURS] = {nodeRowBot, nodeRowUp, nodeColLeft, nodeColRight};
    MPI_Request neighbour_send_req[MAX_NEIGHBOURS];
    MPI_Request neighbour_recieve_req[MAX_NEIGHBOURS];

    MPI_Status neighbour_status[MAX_NEIGHBOURS];
    int recieved_availability[MAX_NEIGHBOURS];
    int counter = 0;
    for (int i = 0; i < MAX_NEIGHBOURS; i++)
    {
        int neighbour_rank = all_neighbour_ranks[i];
        printf("rank %d sending request to %d\n", shared_rank, neighbour_rank);
        MPI_Isend(&temp_val, 1, MPI_INT, neighbour_rank, RESPONSE_TAG, shared_comm, &neighbour_send_req[i]);
        MPI_Irecv(&recieved_availability[i], 1, MPI_INT, neighbour_rank, RESPONSE_TAG, shared_comm, &neighbour_recieve_req[i]);
    }

    MPI_Waitall(MAX_NEIGHBOURS, &neighbour_recieve_req, &neighbour_status);
    for (int i = 0; i < MAX_NEIGHBOURS; i++)
    {
        printf("rank %d recieved %d from neighbour %d\n", shared_rank, recieved_availability[i], all_neighbour_ranks[i]);
    }
    pthread_mutex_lock(&termiate_flag_mutex);
    communication_thread_done = 1;
    pthread_mutex_unlock(&termiate_flag_mutex);
    printf("done communicating\n");
}

int charging_nodes_func(int size, int rank, int base, int ndims, MPI_Comm master_comm, MPI_Comm comm)
{
    MPI_Status base_status, neighbour_status;
    MPI_Request base_req, neighbour_req;
    shared_rank = rank;
    shared_ndims = ndims;
    shared_comm = comm;

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
    int received_value, temp_val = 0, comm_flag = 0;
    MPI_Irecv(&received_value, 1, MPI_INT, base, MPI_ANY_TAG, master_comm, &base_req);
    time_t now = time(NULL);
    struct tm *ptm;
    while (1)
    {
        now = time(NULL);
        ptm = gmtime(&now);

        MPI_Test(&base_req, &flag, &base_status);
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
        // MPI_Test(&neighbour_req, &comm_flag, &neighbour_status);
        MPI_Iprobe(MPI_ANY_SOURCE, RESPONSE_TAG, comm, &comm_flag, &neighbour_status);
        if (comm_flag)
        {
            MPI_Recv(&temp_val, 1, MPI_INT, neighbour_status.MPI_SOURCE, RESPONSE_TAG, comm, &neighbour_status);
            printf("rank %d received request from %d\n", rank, neighbour_status.MPI_SOURCE);
            pthread_mutex_lock(&availabiltiy_counter_mutex);
            MPI_Isend(&shared_availability_counter, 1, MPI_INT, neighbour_status.MPI_SOURCE, RESPONSE_TAG, comm, &neighbour_req);
            pthread_mutex_unlock(&availabiltiy_counter_mutex);

            // set listner again
            MPI_Iprobe(MPI_ANY_SOURCE, RESPONSE_TAG, comm, &comm_flag, &neighbour_status);

            comm_flag = 0;
        }
        if (ptm->tm_sec % interval == 0)
        {

            pthread_mutex_lock(&availabiltiy_counter_mutex);
            int current_availability = shared_availability_counter;
            pthread_mutex_unlock(&availabiltiy_counter_mutex);
            if (current_availability <= NODE_THRESHOLD)
            {
                pthread_create(&communication_thread, NULL, communicate_neighbour_thread_func, (void *)rank);
            }

            pthread_mutex_lock(&charging_logs_mutex);
            insert_new_charging_node(charging_logs, current_availability);
            printf("Printing logs from rank %d\n", rank);
            print_charging_logs(charging_logs);
            pthread_mutex_unlock(&charging_logs_mutex);
        }
        pthread_mutex_lock(&termiate_flag_mutex);

        if (communication_thread_done)
        {
            printf("terminating communication thread\n");
            pthread_join(communication_thread, NULL);
            communication_thread_done = 0;
        }
        pthread_mutex_unlock(&termiate_flag_mutex);
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