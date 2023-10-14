#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include "shared.h"
#include "base.h"
MPI_Comm shared_master_comm;
int *shared_dims;
volatile int share_terminate_flag = 0;
pthread_mutex_t terminate_lock = PTHREAD_MUTEX_INITIALIZER;
int iter = 10;

void *node_communication_thread_function(void *arg)
{
    int node = (int)arg;

    int comm_flag = 0;
    charging_node_status recieved_status;
    MPI_Status status;
    int report_coords[NDIMS];
    char *buffer;
    int buf_size, buf_size_availability, buf_size_neighbour_size, buf_size_neighbours, buf_size_neighbours_availability;
    time_t now;
    int position;
    int temp;
    MPI_Pack_size(1, MPI_INT, shared_master_comm, &buf_size_availability);
    MPI_Pack_size(1, MPI_INT, shared_master_comm, &buf_size_neighbour_size);
    MPI_Pack_size(4, MPI_INT, shared_master_comm, &buf_size_neighbours);
    MPI_Pack_size(4, MPI_INT, shared_master_comm, &buf_size_neighbours_availability);
    buf_size = buf_size_availability + buf_size_neighbour_size + buf_size_neighbours + buf_size_neighbours_availability;
    buffer = (char *)malloc(buf_size);

    for (int i = 0; i < iter; i++)
    {
        // pthread_mutex_lock(&terminate_lock);
        if (share_terminate_flag)
        {
            break;
        }
        // pthread_mutex_unlock(&terminate_lock);
        MPI_Iprobe(node, BASE_COMM_TAG, shared_master_comm, &comm_flag, &status);
        if (comm_flag)
        {
            now = time(NULL);
            position = 0;
            MPI_Recv(buffer, buf_size, MPI_PACKED, node, BASE_COMM_TAG, shared_master_comm, &status);
            int reported_node = status.MPI_SOURCE;
            printf("Base recived status from %d\n", reported_node);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.avilablity, 1, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbour_size, 1, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbours, 4, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbours_avilablity, 4, MPI_INT, shared_master_comm);
            char time_buffer[80];
            rank_to_coords(reported_node, shared_dims, report_coords);
            struct tm *timeinfo = localtime(&now);

            strftime(time_buffer, sizeof(time_buffer), "%a %Y-%m-%d %H:%M:%S", timeinfo);
            printf("Base recived status from %d coord (%d,%d) at %s\n", reported_node, report_coords[0], report_coords[1], time_buffer);
            fflush(stdout);
            printf("Reporting Node %d number of adjacent node: %d available port: %d\n", reported_node, recieved_status.neighbour_size, recieved_status.avilablity);
            for (int i = 0; i < recieved_status.neighbour_size; i++)
            {
                printf("Reporting Node %d neighbour %d available port: %d\n", reported_node, recieved_status.neighbours[i], recieved_status.neighbours_avilablity[i]);
            }
            fflush(stdout);
        }
        comm_flag = 0;
        sleep(1);
        printf("Base is waiting for status from %d\n", node);
    }
    free(buffer);

    printf("terminating thread from base");
}

int base_func(int base, MPI_Comm master_comm, int *dims, int num_chraging_nodes)
{
    printf("Base is running with %d\n", num_chraging_nodes);
    int recbuf;
    // shared_dims = dims;
    int comm_flag;
    MPI_Status incoming_status;
    pthread_t comm_thread[num_chraging_nodes];
    shared_master_comm = master_comm;
    shared_dims = dims;
    int current_iteration = 0;
    for (int node = 0; node < num_chraging_nodes; node++)
    {
        pthread_create(&comm_thread[node], NULL, node_communication_thread_function, (void *)node);
    }

    time_t now;

    for (int i = 0; i < iter; i++)
    {
        current_iteration++;
        printf("current iteration %d\n", current_iteration);
        sleep(1);
    }

    share_terminate_flag = 1;

    for (int i = 0; i < num_chraging_nodes; i++)
    {
        pthread_join(comm_thread[i], NULL);
    }
    int term = TERMINATE_TAG;
    for (int node = 0; node < base; node++)
    {
        MPI_Send(&term, 1, MPI_INT, node, TERMINATE_TAG, master_comm);
    }

    return 0;
}

void rank_to_coords(int rank, int *dims, int *coords)
{
    coords[0] = rank / dims[1];
    coords[1] = rank % dims[1];
}

int get_cart_neighbors(int rank, int *dims, int *neighbors)
{
    int coords[NDIMS];
    int size = 0; // Number of neighbors

    // Calculate coordinates for the given rank
    rank_to_coords(rank, dims, coords);

    // Check Left neighbor
    if (coords[1] > 0)
    {
        coords[1]--;
        neighbors[size++] = coords[0] * dims[1] + coords[1];
        coords[1]++; // Reset to original
    }

    // Check Right neighbor
    if (coords[1] < dims[1] - 1)
    {
        coords[1]++;
        neighbors[size++] = coords[0] * dims[1] + coords[1];
        coords[1]--; // Reset to original
    }

    // Check Top neighbor
    if (coords[0] > 0)
    {
        coords[0]--;
        neighbors[size++] = coords[0] * dims[1] + coords[1];
        coords[0]++; // Reset to original
    }

    // Check Bottom neighbor
    if (coords[0] < dims[0] - 1)
    {
        coords[0]++;
        neighbors[size++] = coords[0] * dims[1] + coords[1];
    }

    return size;
}