#define NDIMS 2
enum MessageTags
{
    REQUEST_TAG,
    RESPONSE_TAG,
    TERMINATE_TAG,
    BASE_COMM_TAG
};
struct
{
    int avilablity;
    int neighbour_size;
    int neighbours[4];
    int neighbours_avilablity[4];

} typedef charging_node_status;
