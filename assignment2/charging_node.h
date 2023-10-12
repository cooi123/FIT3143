#include <mpi.h>
#define TERMINATE_VALUE -1
int charging_nodes_func(int size, int rank, int base, int ndims, MPI_Comm master_comm, MPI_Comm comm);
int initialise_charging_grid(int size, int rank, int ndims, int *dims, MPI_Comm existing_comm, MPI_Comm *new_comm);
int incoming_car_simulator(int rank);
enum MessageTags
{
    REQUEST_TAG,
    RESPONSE_TAG,
    TERMINATE_TAG
};
struct
{
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
    int avilablity
} typedef charging_node;

struct
{
    charging_node logs[10];
    int size;
    int start_index;
    int end_index;
} typedef charging_node_logs;
