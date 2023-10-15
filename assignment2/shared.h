#define NDIMS 2

enum MessageTags
{
    REQUEST_TAG,
    RESPONSE_TAG,
    TERMINATE_TAG,
    NODE_BASE_COMM_TAG,
    BASE_NODE_COMM_TAG,
    NODE_NODE_COMM_TAG,

};
struct
{
    int avilablity;
    int neighbour_size;
    int neighbours[4];
    int neighbours_avilablity[4];
    double start;

} typedef charging_node_status;
