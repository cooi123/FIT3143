#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include "shared.h"
#include "base.h"

MPI_Comm shared_master_comm;
int *shared_dims;
volatile int share_terminate_flag = 0;
int iter = 20;
FILE *file;
int current_iteration = 0;
int *previous_reported_nodes;
int num_chraging_nodes;
void *node_communication_thread_function(void *arg)
{
    int node = (int)arg;

    int comm_flag = 0;
    charging_node_status recieved_status;
    MPI_Status status;
    int report_coords[NDIMS];
    char *buffer;
    int buf_size, buf_size_availability, buf_size_neighbour_size, buf_size_neighbours, buf_size_neighbours_availability, buf_size_time;
    time_t now;
    int position;
    int temp;
    MPI_Pack_size(1, MPI_INT, shared_master_comm, &buf_size_availability);
    MPI_Pack_size(1, MPI_INT, shared_master_comm, &buf_size_neighbour_size);
    MPI_Pack_size(4, MPI_INT, shared_master_comm, &buf_size_neighbours);
    MPI_Pack_size(4, MPI_INT, shared_master_comm, &buf_size_neighbours_availability);
    MPI_Pack_size(1, MPI_DOUBLE, shared_master_comm, &buf_size_time);
    buf_size = buf_size_availability + buf_size_neighbour_size + buf_size_neighbours + buf_size_neighbours_availability + buf_size_time;
    buffer = (char *)malloc(buf_size);
    double start, end, communication_time;
    while (1)
    {

        if (share_terminate_flag)
        {
            break;
        }

        MPI_Iprobe(node, NODE_BASE_COMM_TAG, shared_master_comm, &comm_flag, &status);
        if (comm_flag)
        {
            now = time(NULL);
            position = 0;
            previous_reported_nodes[node] = 1;
            MPI_Recv(buffer, buf_size, MPI_PACKED, node, NODE_BASE_COMM_TAG, shared_master_comm, &status);
            int reported_node = status.MPI_SOURCE;
            fprintf(file, "iteration %d Base recived report from %d\n", current_iteration, reported_node);
            struct tm *timeinfo = localtime(&now);
            char time_buffer[80];
            strftime(time_buffer, sizeof(time_buffer), "%a %Y-%m-%d %H:%M:%S", timeinfo);

            fprintf(file, "Alert time for %d coord (%d,%d) : %s\n", reported_node, report_coords[0], report_coords[1], time_buffer);

            MPI_Unpack(buffer, buf_size, &position, &recieved_status.avilablity, 1, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbour_size, 1, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbours, 4, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.neighbours_avilablity, 4, MPI_INT, shared_master_comm);
            MPI_Unpack(buffer, buf_size, &position, &recieved_status.start, 1, MPI_DOUBLE, shared_master_comm);

            start = recieved_status.start;
            end = MPI_Wtime();
            communication_time = fabs(end - start);

            printf("Reporting Node %d number of adjacent node: %d available port: %d\n", reported_node, recieved_status.neighbour_size, recieved_status.avilablity);
            int neighbour_coords[2], neighbour_size, nearby_neighbours[4], available_station[num_chraging_nodes];

            for (int i = 0; i < recieved_status.neighbour_size; i++)
            {
                rank_to_coords(recieved_status.neighbours[i], shared_dims, neighbour_coords);
                fprintf(file, "Adjacent Nodes %d neighbour coor (%d,%d) %d available port: %d\n", recieved_status.neighbours[i], neighbour_coords[0], neighbour_coords[1], recieved_status.neighbours_avilablity[i]);
                neighbour_size = get_cart_neighbors(recieved_status.neighbours[i], shared_dims, nearby_neighbours);
                for (int j = 0; j < neighbour_size; j++)
                {
                    fprintf(file, "Nearby Nodes %d\n", nearby_neighbours[j]);
                    // check recent report from nearby nodes

                    if (previous_reported_nodes[nearby_neighbours[j]] == 0)
                    {

                        available_station[nearby_neighbours[j]] = 1;
                    }
                }
            }
            fprintf(file, "Available station (no report in last %d seconds):", K);
            for (int i = 0; i < num_chraging_nodes; i++)
            {
                if (available_station[i] == 1)
                {
                    fprintf(file, "%d ", i);
                    MPI_Isend(&available_station[i], 1, MPI_INT, node, BASE_NODE_COMM_TAG, shared_master_comm, &status);
                }
            }
            fprintf(file, "Communication time(seconds): %.2f\n", communication_time);
            fprintf(file, "Total Messages send between reporting node and base: %d\n", 2);
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
    int comm_flag;
    num_chraging_nodes = num_chraging_nodes;
    previous_reported_nodes = (int *)malloc(sizeof(int) * num_chraging_nodes);
    MPI_Status incoming_status;
    pthread_t comm_thread[num_chraging_nodes];
    shared_master_comm = master_comm;
    shared_dims = dims;
    const char *dirname = "output_logs";
    char mkdir_cmd[200];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p %s", dirname);
    system(mkdir_cmd);
    char filename[200];
    snprintf(filename, sizeof(filename), "%s/base_node_log.txt", dirname);

    file = fopen(filename, "w");
    fprintf(file, "Starting base log\n");
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
    fclose(file);
    free(previous_reported_nodes);
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