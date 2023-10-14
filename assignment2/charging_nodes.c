#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "charging_node.h"
#include "shared.h"
#include <pthread.h>
const char *dirname = "output_logs";
#define NODE_THRESHOLD 1
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISPLACEMENT 1
#define MAX_NEIGHBOURS 4
#define INTERVAL 2

int shared_availability_counter = 0;
charging_node_logs *charging_logs;
pthread_mutex_t availabiltiy_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t charging_logs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t termiate_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int terminate_flag = 0;
volatile int communication_thread_done = 0;
int shared_ndims;
int shared_rank;
int shared_base_rank;
int node_availability[K] = {0};

MPI_Comm shared_comm;
MPI_Comm shared_main_comm;
int initialise_charging_grid(int size, int rank, int ndims, int *dims, MPI_Comm existing_comm, MPI_Comm *new_comm)
{

    int periods[2] = {0, 0}; // Non-periodic grid
    int reorder = 0;         // preserve original ordering

    if (rank == 0)
        printf("Comm Size: %d: Grid Dimension =[%d x %d] \n", size, dims[0], dims[1]);

    int ierr = MPI_Cart_create(existing_comm, ndims, dims, periods, reorder, new_comm);
    if (ierr != 0)
    {
        printf("ERROR[%d] creating CART\n", ierr);
        MPI_Finalize();
        exit(0);
    }
    char mkdir_cmd[200];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", dirname);
    system(mkdir_cmd);
    char filename[200];
    snprintf(filename, sizeof(filename), "%s/charging_logs_rank_%d.txt", dirname, rank);

    FILE *file = fopen(filename, "w");
    fprintf(file, "Starting log\n", rank);
    fclose(file);
    return ierr;
}
void *thread_function(void *arg)
{
    while (!terminate_flag)
    {
        srand(time(NULL) + (int)arg);
        int sleep_time = rand() % 5;
        sleep(sleep_time);

        int is_available = rand() % 2;
        node_availability[(int)arg] = is_available;
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

    int all_neighbour_ranks[MAX_NEIGHBOURS] = {nodeRowBot, nodeRowUp, nodeColLeft, nodeColRight};
    MPI_Request neighbour_send_req[MAX_NEIGHBOURS];
    MPI_Request neighbour_recieve_req[MAX_NEIGHBOURS];

    MPI_Status neighbour_status[MAX_NEIGHBOURS];
    int recieved_availability[MAX_NEIGHBOURS];
    int counter = 0;

    char filename[200];
    snprintf(filename, sizeof(filename), "%s/charging_logs_rank_%d.txt", dirname, shared_rank);

    FILE *log_file = fopen(filename, "a");
    if (!log_file)
    {
        fprintf(stderr, "Failed to open file for rank %d\n", shared_rank);
    }

    for (int i = 0; i < MAX_NEIGHBOURS; i++)
    {
        int neighbour_rank = all_neighbour_ranks[i];
        MPI_Isend(&temp_val, 1, MPI_INT, neighbour_rank, RESPONSE_TAG, shared_comm, &neighbour_send_req[i]);
        MPI_Irecv(&recieved_availability[i], 1, MPI_INT, neighbour_rank, RESPONSE_TAG, shared_comm, &neighbour_recieve_req[i]);
        if (neighbour_rank != -2)
        {

            fprintf(log_file, "rank %d sent request for availablity to %d\n", shared_rank, neighbour_rank);
        }
    }
    fclose(log_file);
    MPI_Waitall(MAX_NEIGHBOURS, &neighbour_recieve_req, &neighbour_status);
    charging_node_status node_status;
    node_status.avilablity = charging_logs->logs[charging_logs->end_index].avilablity;
    node_status.neighbour_size = 0;

    int is_all_available = 0;
    log_file = fopen(filename, "a");
    if (!log_file)
    {
        fprintf(stderr, "Failed to open file for rank %d\n", shared_rank);
    }
    for (int i = 0; i < MAX_NEIGHBOURS; i++)
    {
        if (all_neighbour_ranks[i] != -2)
        {
            fprintf(log_file, "rank %d recieved availablity of %d from %d\n", shared_rank, recieved_availability[i], all_neighbour_ranks[i]);

            if (recieved_availability[i] > NODE_THRESHOLD)
            {
                is_all_available = 1;
                break;
            }
            node_status.neighbours[node_status.neighbour_size] = all_neighbour_ranks[i];
            node_status.neighbours_avilablity[node_status.neighbour_size] = recieved_availability[i];
            node_status.neighbour_size++;
        }
    }
    fclose(log_file);

    if (!is_all_available)
    {
        double start = MPI_Wtime();
        printf("%dBase comm %d at %lf\n", shared_rank, shared_base_rank, start);
        node_status.start = start;
        char *buffer;
        int buf_size, buf_size_availability, buf_size_neighbour_size, buf_size_neighbours, buf_size_neighbours_availability, buf_size_time;
        MPI_Pack_size(1, MPI_INT, shared_main_comm, &buf_size_availability);
        MPI_Pack_size(1, MPI_INT, shared_main_comm, &buf_size_neighbour_size);
        MPI_Pack_size(4, MPI_INT, shared_main_comm, &buf_size_neighbours);
        MPI_Pack_size(4, MPI_INT, shared_main_comm, &buf_size_neighbours_availability);
        MPI_Pack_size(1, MPI_DOUBLE, shared_main_comm, &buf_size_time);
        buf_size = buf_size_availability + buf_size_neighbour_size + buf_size_neighbours + buf_size_neighbours_availability + buf_size_time;
        buffer = (char *)malloc(buf_size);
        int position = 0;
        MPI_Pack(&node_status.avilablity, 1, MPI_INT, buffer, buf_size, &position, shared_main_comm);
        MPI_Pack(&node_status.neighbour_size, 1, MPI_INT, buffer, buf_size, &position, shared_main_comm);
        MPI_Pack(&node_status.neighbours, 4, MPI_INT, buffer, buf_size, &position, shared_main_comm);
        MPI_Pack(&node_status.neighbours_avilablity, 4, MPI_INT, buffer, buf_size, &position, shared_main_comm);
        MPI_Pack(&node_status.start, 1, MPI_DOUBLE, buffer, buf_size, &position, shared_main_comm);
        MPI_Send(buffer, buf_size, MPI_PACKED, shared_base_rank, NODE_BASE_COMM_TAG, shared_main_comm);
        free(buffer);
        // MPI_Send(&node_status.avilablity, 1, MPI_INT, shared_base_rank, BASE_COMM_TAG, shared_main_comm);
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
    shared_main_comm = master_comm;
    shared_base_rank = base;

    int my_coords[ndims];
    charging_logs = (charging_node_logs *)malloc(sizeof(charging_node_logs));
    charging_logs->size = 0;
    charging_logs->start_index = 0;
    charging_logs->end_index = -1;

    MPI_Cart_coords(comm, rank, ndims, my_coords);

    printf("Process %d has coordinates (%d, %d)\n", rank, my_coords[0], my_coords[1]);
    // send to base

    int arr[3] = {0};
    int sum = 0, current_iter = 0;
    int flag;
    pthread_t threads[K];
    pthread_t communication_thread;
    for (int i = 0; i < K; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, (void *)i);
    }
    int received_value, temp_val = 0, comm_flag = 0;
    MPI_Irecv(&received_value, 1, MPI_INT, base, MPI_ANY_TAG, master_comm, &base_req);
    while (1)
    {
        MPI_Test(&base_req, &flag, &base_status);
        if (flag)
        {
            switch (base_status.MPI_TAG)
            {
            case TERMINATE_TAG:
                printf("Process %d received termination signal.\n", rank);
                terminate_flag = 1;
                for (int i = 0; i < K; i++)
                {
                    pthread_join(threads[i], NULL);
                }
                printf("Process %d terminated.\n", rank);
                break;

            case BASE_NODE_COMM_TAG:
                printf("Next available charging station is %d\n", received_value);
                break;
            }
            MPI_Irecv(&received_value, 1, MPI_INT, base, MPI_ANY_TAG, master_comm, &base_req);
        }
        // MPI_Test(&neighbour_req, &comm_flag, &neighbour_status);
        MPI_Iprobe(MPI_ANY_SOURCE, RESPONSE_TAG, comm, &comm_flag, &neighbour_status);
        while (comm_flag)
        {
            MPI_Recv(&temp_val, 1, MPI_INT, neighbour_status.MPI_SOURCE, RESPONSE_TAG, comm, &neighbour_status);
            printf("rank %d received request for availability from %d\n", rank, neighbour_status.MPI_SOURCE);
            pthread_mutex_lock(&charging_logs_mutex);
            MPI_Isend(&charging_logs->logs[charging_logs->end_index].avilablity, 1, MPI_INT, neighbour_status.MPI_SOURCE, RESPONSE_TAG, comm, &neighbour_req);
            pthread_mutex_unlock(&charging_logs_mutex);
            comm_flag = 0;

            // set listner again
            MPI_Iprobe(MPI_ANY_SOURCE, RESPONSE_TAG, comm, &comm_flag, &neighbour_status);
        }
        if (terminate_flag)
        {
            return 0;
        }
        if (current_iter % INTERVAL == 0)
        {
            // get current availabiltiy
            pthread_mutex_lock(&availabiltiy_counter_mutex);
            int current_availability = 0;
            for (int i = 0; i < K; i++)
            {
                current_availability += 1 - node_availability[i];
            }
            pthread_mutex_unlock(&availabiltiy_counter_mutex);
            pthread_mutex_lock(&charging_logs_mutex);
            insert_new_charging_node(charging_logs, current_availability, current_iter);

            write_to_file(rank, charging_logs, current_iter);
            pthread_mutex_unlock(&charging_logs_mutex);
            if (current_availability <= NODE_THRESHOLD)
            {
                pthread_create(&communication_thread, NULL, communicate_neighbour_thread_func, (void *)rank);
                pthread_join(communication_thread, NULL);
            }
        }
        sleep(1);

        pthread_mutex_lock(&termiate_flag_mutex);

        if (communication_thread_done)
        {
            printf("terminating communication thread\n");
            pthread_join(communication_thread, NULL);
            communication_thread_done = 0;
        }
        pthread_mutex_unlock(&termiate_flag_mutex);
        current_iter++;
    }
    free(charging_logs);
}

int insert_new_charging_node(charging_node_logs *logs, int availablity, int iter)
{
    charging_node node;
    get_time(&node.second, &node.minute, &node.hour, &node.day, &node.month, &node.year);
    node.avilablity = availablity;
    printf("iteration %d rank %d availabilty: %d\n", iter, shared_rank, node.avilablity);

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

int write_to_file(int rank, charging_node_logs *logs, int iter)
{

    char filename[200];
    snprintf(filename, sizeof(filename), "%s/charging_logs_rank_%d.txt", dirname, rank);

    FILE *log_file = fopen(filename, "a");
    if (log_file)
    {
        fprintf(log_file, "current log for rank %d iteration: %d\n", rank, iter);
        int start_index = logs->start_index;
        for (int i = 0; i < logs->size; i++)
        {
            fprintf(log_file, "%d/%d/%d %d:%d:%d, %d\n", logs->logs[start_index].day, logs->logs[start_index].month, logs->logs[start_index].year, logs->logs[start_index].hour, logs->logs[start_index].minute, logs->logs[start_index].second, logs->logs[start_index].avilablity);
            start_index = (start_index + 1) % 10;
        }
        fclose(log_file);
    }
    else
    {
        fprintf(stderr, "Failed to open file for rank %d\n", rank);
    }
    return 0;
}
