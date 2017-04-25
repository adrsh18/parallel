#include<stdio.h>
#include "mpi.h"
#include "logger.h"
#include "utils.h"

int rank, size;

extern const char *INFO, *WARN, *ERROR;

int main(int argc, char **argv)
{
    int i, ierr, sum, endPoint;
    int data[1];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    logger(INFO, "Begin");
    sum = rank;

    for(i = 1; i <= numRecursiveHalvingPasses(size); i++) {
        
        if(rank % power(2, i) == 0) {
            endPoint = rank + power(2, i-1);
            printf("%s Receiving from %d\n", logger_string(INFO), endPoint);
            MPI_Recv(data, 1, MPI_INT, endPoint, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            sum = sum + data[0];
            printf("%s Sum now is %d\n", logger_string(INFO), sum);
        } else if(rank % power(2, i) == power(2, i-1)) {
            endPoint = rank - power(2, i-1);
            printf("%s Sending to %d\n", logger_string(INFO), endPoint);
            data[0] = sum;
            MPI_Send(data, 1, MPI_INT, endPoint, 11, MPI_COMM_WORLD);
        }
    }
    
    ierr = MPI_Finalize();

    logger(INFO, "End");

    return 0;
}
