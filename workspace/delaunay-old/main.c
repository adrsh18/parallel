#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "logger.h"
#include "utils.h"
#include "structures.h"
#include "delaunay.h"
#include <time.h>

int rank, size;

extern const char *INFO, *WARN, *ERROR;

Point *points;
Edge *edges;
long edgeCounter = 0;
long nPoints;
long nEdges;

void readOrFail(int argc, char **argv);
void clear(Point **point);
//void merge(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *lowerCommonTangentIdx);


int main(int argc, char **argv)
{
    int p, ierr, sum, endPoint, i;
//    int data[1];
    MPI_Status status;
    
    MPI_Datatype MPIPoint, MPIEdge;
    Point samplePoint[0];
    Edge sampleEdge[0];
    int dSize = sizeof(double);
    MPI_Datatype pointTypes[3] = {MPI_FLOAT, MPI_FLOAT, MPI_LONG}, edgeTypes[6] = {MPI_LONG, MPI_LONG, MPI_LONG, MPI_LONG, MPI_LONG, MPI_LONG};
    int pointBlocklen[3] = {1,1,1}, edgeBlocklen[6] = {1,1,1,1,1,1};
    MPI_Aint pointDisp[3] = {0, sizeof(double), 2*sizeof(double) };
    MPI_Aint  edgeDisp[6] = {0, dSize, 2*dSize, 3*dSize, 4*dSize, 5*dSize};
    
    long leftEdgeIdx, rightEdgeIdx;
    long lowerCommonTangentIdx;
    clock_t timerBegin, timerEnd;
    double timeElapsed;

    timerBegin = clock();

    readOrFail(argc, argv);

    timerEnd = clock();
    
    timeElapsed = (double) (timerEnd - timerBegin) / CLOCKS_PER_SEC;
    printf("%s Finished reading input in %lf seconds\n", logger_string(INFO), timeElapsed); 

    MPI_Init(&argc, &argv);

    MPI_Type_create_struct(3, pointBlocklen, pointDisp, pointTypes, &MPIPoint);
    MPI_Type_commit(&MPIPoint);

    MPI_Type_create_struct(6, edgeBlocklen, edgeDisp, edgeTypes, &MPIEdge);
    MPI_Type_commit(&MPIEdge);


    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    logger(INFO, "Begin");

    long data[10];

    long dataPerP = nPoints / size;
    long myStart = rank * dataPerP;
    long myEnd = myStart + dataPerP;
    long neighborStart, neighborEnd;
    long nNeighborEdges;
    long neighborLeftEdgeIdx, neighborRightEdgeIdx;

    //sum = rank;
    Point *neighborPoints;
    Edge *neighborEdges;

    clock_t localDTBegin = clock();

    //for(i = myStart; i < myEnd; i++) points[i].entryPoint = rank;
    delaunay(myStart, myEnd-1, &leftEdgeIdx, &rightEdgeIdx);
    printf("%s Number of Edges: %ld\n", logger_string(INFO), edgeCounter);

    timerEnd = clock();
    timeElapsed = (double) (timerEnd - localDTBegin) / CLOCKS_PER_SEC;
    printf("%s Finished triangulation of local points in %lf seconds\n", logger_string(INFO), timeElapsed);

    clock_t mergeBegin = clock();

    for(p = 1; p <= numRecursiveHalvingPasses(size); p++) {
        
        if(rank % power(2, p) == 0) {
            endPoint = rank + power(2, p-1);
            if(endPoint >= size) { logger(INFO, "Nothing to do in this pass"); continue; }
            neighborStart = endPoint * dataPerP;
            neighborEnd = neighborStart + (power(2, p-1) * dataPerP);
            neighborPoints = malloc(sizeof(Point) * (neighborEnd - neighborStart));
            printf("%s Receiving from %d\n", logger_string(INFO), endPoint);
            MPI_Recv(neighborPoints, neighborEnd - neighborStart, MPIPoint, endPoint, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            //MPI_Recv(neighborEdges, nNeighborEdges, MPIEdge, endPoint, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            //sum = sum + data[0];
            long offset = edgeCounter;
            for(i = 0; i < (neighborEnd-neighborStart); i++) {points[neighborStart+i].entryPoint = neighborPoints[i].entryPoint + offset; }
            MPI_Recv(data, 3, MPI_LONG, endPoint, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            nNeighborEdges = data[0];
            neighborLeftEdgeIdx = data[1]+offset; neighborRightEdgeIdx = data[2]+offset;
            neighborEdges = malloc(sizeof(Edge) * nNeighborEdges);
            printf("%s About to receive %ld edges\n", logger_string(INFO), nNeighborEdges);
            MPI_Recv(neighborEdges, nNeighborEdges, MPIEdge, endPoint, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            for(i = 0; i < nNeighborEdges; i++) {
                if(neighborEdges[i].origin == -1 || neighborEdges[i].destination == -1) { edgeCounter++;continue; }
                //printEdge(edgeCounter);
                edges[edgeCounter].origin = neighborEdges[i].origin;
                edges[edgeCounter].destination = neighborEdges[i].destination;

                edges[edgeCounter].originNext = neighborEdges[i].originNext + offset;
                edges[edgeCounter].originPrev = neighborEdges[i].originPrev + offset;
                edges[edgeCounter].destinationNext = neighborEdges[i].destinationNext + offset;
                edges[edgeCounter].destinationPrev = neighborEdges[i].destinationPrev + offset;
                //printEdge(edgeCounter);
                edgeCounter++;
            }
            free(neighborPoints);
            free(neighborEdges); 
            logger(INFO, "About to merge both regions");

            merge(rightEdgeIdx, myEnd-1, neighborLeftEdgeIdx, neighborStart, &lowerCommonTangentIdx);
           
            logger(INFO, "Done merging :)");
        
            if(edges[lowerCommonTangentIdx].origin == myStart) {
                leftEdgeIdx = lowerCommonTangentIdx;
            } 
            if(edges[lowerCommonTangentIdx].destination == neighborEnd-1) {
                neighborRightEdgeIdx = lowerCommonTangentIdx;
            }

            rightEdgeIdx = neighborRightEdgeIdx;
            logger(INFO, "Finished re-assignments");
            //printf("%s myEnd: %ld; neighborStart: %ld\n", logger_string(INFO), myEnd, neighborStart);
            myEnd = neighborEnd;
        } else if(rank % power(2, p) == power(2, p-1)) {
            endPoint = rank - power(2, p-1);
            printf("%s Sending to %d\n", logger_string(INFO), endPoint);
            data[0] = edgeCounter; data[1] = leftEdgeIdx; data[2] = rightEdgeIdx;
            //logger(INFO, "Last point");
            //printPoint(myEnd-1);
            MPI_Send(&points[myStart], myEnd-myStart, MPIPoint, endPoint, 11, MPI_COMM_WORLD);
            MPI_Send(data, 3, MPI_LONG, endPoint, 11, MPI_COMM_WORLD);
            printf("%s About to send %ld edges\n", logger_string(INFO),edgeCounter);
            MPI_Send(edges, edgeCounter, MPIEdge, endPoint, 11, MPI_COMM_WORLD);
        }
    }

    timerEnd = clock();
    timeElapsed = (double) (timerEnd - mergeBegin) / CLOCKS_PER_SEC;
    printf("%s Finished merging in %lf seconds\n", logger_string(INFO), timeElapsed);


    timeElapsed = (double) (timerEnd - timerBegin) / CLOCKS_PER_SEC; 
    printf("%s Total execution time is %lf seconds\n", logger_string(INFO), timeElapsed);

    timeElapsed = (double) (timerEnd - localDTBegin) / CLOCKS_PER_SEC;
    printf("%s Actual Computation Time (with comm.cost) is %lf seconds\n", logger_string(INFO), timeElapsed);

    if(rank == 0) {
        //for(i = 0; i < edgeCounter; i++) printEdge(i);
        //for(i = 0; i < nPoints; i++) printPoint(i);
        writeEdges(argv[3],myStart, myEnd);
    }   
 
    ierr = MPI_Finalize();
    //if(rank == 0) for(i=0;i<nPoints;i++) printf("%d\n", points[i].entryPoint);
    //printEdges(myStart, myEnd);

    logger(INFO, "End");

    return 0;
}

void readOrFail(int argc, char **argv)
{

    if(argc < 4) {
        printf("Usage: main <data_file_as_csv> <num_of_points> <name_of_output_file>\n");
        exit(1);
    }

    char *inputFileName = argv[1];
    long numPoints = atol(argv[2]);
    long temp, i = 0,j=0;
    long numEdges = 0;

    FILE *inputFile = fopen(inputFileName, "r");

    points = malloc(sizeof(Point) * numPoints);
    if(points == NULL)
    {
        printf("Failed to allocate memory!\n");
        exit(3);
    }

    for(j=0;j<numPoints;j++) {
        points[j].x = -1.00;
        points[j].y = -1.00;
        points[j].entryPoint = -1;
    }

    while(fscanf(inputFile, "%ld,%lf,%lf", &temp, &points[i].x, &points[i].y) == 3)
    {
        i++;
    }
    fclose(inputFile);

    printf("%ld points read into memory from file.\n", i);
    numEdges = logTwo(numPoints)*numPoints - 6;
    edges = malloc(sizeof(Edge) * numEdges);
    for(j=0;j<numEdges;j++) {
        edges[j].origin = -1;
        edges[j].destination = -1;
        edges[j].originNext = -1;
        edges[j].originPrev = -1;
        edges[j].destinationNext = -1;
        edges[j].destinationPrev = -1;
    }

    nPoints = numPoints;
    nEdges = numEdges;

}
