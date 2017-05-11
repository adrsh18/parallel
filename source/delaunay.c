#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "delaunay.h"


extern Point *points;
extern Edge *edges;
extern long edgeCounter;
extern long nPoints;
extern long nEdges;

extern char *INFO, *WARN, *ERROR;

/*
void delaunay(long start, long end, long *leftEdgeIdx, long *rightEdgeIdx);
long createEdge(long startPointIdx, long endPointIdx);
void addEdgeToRing(long edgeOneIdx, long edgeTwoIdx, long commonPointIdx);
double triangleCheck(long pointOneIdx, long pointTwoIdx, long pointThreeIdx);
long makeTriangle(long edgeOneIdx, long pointOneIdx, long edgeTwoIdx, long pointTwoIdx, int side);
static void merge(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *lowerCommonTangentIdx);
static void findLowerTangent(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *leftLowerEdgeIdx, long *lowerLowerOriginIdx, long *rightLowerEdgeIdx, long *rightLowerOriginIdx);

void printEdges();
void printEdge();
void printPoint(long p);
void deleteEdge(long e);


int main(int argc, char* argv[1])
{
    long leftEdgeIdx, rightEdgeIdx;
    if(argc != 2) {
        printf("Usage: dtpp <data_file_as_csv>\n");
        exit(1);
    }

    char *inputFileName = argv[1];
    long temp, i = 0,j=0;
    long numEdges = 0;

    FILE *inputFile = fopen(inputFileName, "r");

    points = malloc(sizeof(Point) * COUNT);
    if(points == NULL)
    {
        printf("Failed to allocate memory!\n");
        exit(3);
    }

    for(j=0;j<COUNT;j++) {
        points[j].x = -1.00;
        points[j].y = -1.00;
        points[j].entryPoint = -1;
    }

    while(fscanf(inputFile, "%ld,%f,%f", &temp, &points[i].x, &points[i].y) == 3)
    {
        i++;
    }
    fclose(inputFile);

    printf("%ld points read into memory from file.\n", i);
    numEdges = 3*i - 6;
    edges = malloc(sizeof(Edge) * numEdges);
    for(j=0;j<numEdges;j++) {
        edges[j].origin = -1;
        edges[j].destination = -1;
        edges[j].originNext = -1;
        edges[j].originPrev = -1;
        edges[j].destinationNext = -1;
        edges[j].destinationPrev = -1;
    }


    for(i=0; i<COUNT; i++)
    {
        printf("(%f, %f)\n", points[i].x, points[i].y);
    }

    delaunay(0, COUNT-1, &leftEdgeIdx, &rightEdgeIdx);
    printEdges();
    return 0;
}
*/

void delaunay(long start, long end, long *leftEdgeIdx, long *rightEdgeIdx)
{
    long edgeOneIdx, edgeTwoIdx, edgeThreeIdx;
    long leftSubLeftEdgeIdx, leftSubRightEdgeIdx, rightSubLeftEdgeIdx, rightSubRightEdgeIdx, lowerCommonTangentIdx;
    long pointCount = end - start + 1;
    if(pointCount == 2) {
        *leftEdgeIdx = *rightEdgeIdx = createEdge(start, end);
    } else if(pointCount == 3) {
            edgeOneIdx = createEdge(start, start+1);
            edgeTwoIdx = createEdge(start+1, end);
            addEdgeToRing(edgeOneIdx, edgeTwoIdx, start+1);
            double direction = triangleCheck(start, start+1, end);

            if(direction > 0.0) {
                edgeThreeIdx = makeTriangle(edgeOneIdx, start, edgeTwoIdx, end, 1);
                *leftEdgeIdx = edgeOneIdx;
                *rightEdgeIdx = edgeTwoIdx;
            } else if(direction < 0.0) {
                edgeThreeIdx = makeTriangle(edgeOneIdx, start, edgeTwoIdx, end, 0);
                *leftEdgeIdx = edgeThreeIdx;
                *rightEdgeIdx = edgeThreeIdx;
            } else {
                *leftEdgeIdx = edgeOneIdx;
                *rightEdgeIdx = edgeTwoIdx;
            }
    } else if(pointCount > 3) {
        int mid = start + ((end - start)/2);

        delaunay(start, mid, &leftSubLeftEdgeIdx, &rightSubLeftEdgeIdx);
        delaunay(mid+1, end, &leftSubRightEdgeIdx, &rightSubRightEdgeIdx);

        merge(rightSubLeftEdgeIdx, mid, leftSubRightEdgeIdx, mid+1, &lowerCommonTangentIdx);
        //COME BACK HERE, if MUTATING IN MERGE

        if(edges[lowerCommonTangentIdx].origin == start) {
            leftSubLeftEdgeIdx = lowerCommonTangentIdx;
        }
        if(edges[lowerCommonTangentIdx].destination == end) {
            rightSubRightEdgeIdx = lowerCommonTangentIdx;
        }

        *leftEdgeIdx = leftSubLeftEdgeIdx;
        *rightEdgeIdx = rightSubRightEdgeIdx;

    }
}

long createEdge(long startPointIdx, long endPointIdx)
{
    //printf("Creating Edge between %ld and %ld\n", startPointIdx, endPointIdx);
    long c = edgeCounter;
    edges[c].origin = startPointIdx;
    edges[c].destination = endPointIdx;
    
    edges[c].originNext = edges[c].originPrev = edges[c].destinationNext = edges[c].destinationPrev = c;
    
    if(points[startPointIdx].entryPoint == -1) {
        points[startPointIdx].entryPoint = c;
    }
    if(points[endPointIdx].entryPoint == -1) {
        points[endPointIdx].entryPoint = c;
    }

    edgeCounter++;
    //printEdge(c);
    return c;
}

void addEdgeToRing(long edgeOneIdx, long edgeTwoIdx, long commonPointIdx)
{
    long tempEdgeIdx;
    
    if(edges[edgeOneIdx].origin == commonPointIdx) {
        tempEdgeIdx = edges[edgeOneIdx].originNext;
        edges[edgeOneIdx].originNext = edgeTwoIdx;
    } else {
        tempEdgeIdx = edges[edgeOneIdx].destinationNext;
        edges[edgeOneIdx].destinationNext = edgeTwoIdx;
    }
    if(edges[tempEdgeIdx].origin == commonPointIdx) {
        edges[tempEdgeIdx].originPrev = edgeTwoIdx;
    } else {
        edges[tempEdgeIdx].destinationPrev = edgeTwoIdx;
    }
    if(edges[edgeTwoIdx].origin == commonPointIdx) {
        edges[edgeTwoIdx].originNext = tempEdgeIdx;
        edges[edgeTwoIdx].originPrev = edgeOneIdx;
    } else {
        edges[edgeTwoIdx].destinationNext = tempEdgeIdx;
        edges[edgeTwoIdx].destinationPrev = edgeOneIdx;
    }

}

double triangleCheck(long pointOneIdx, long pointTwoIdx, long pointThreeIdx)
{
    return ( (points[pointTwoIdx].x - points[pointOneIdx].x) * (points[pointThreeIdx].y - points[pointOneIdx].y) 
    - (points[pointTwoIdx].y - points[pointOneIdx].y) * (points[pointThreeIdx].x - points[pointOneIdx].x));

}


long makeTriangle(long edgeOneIdx, long pointOneIdx, long edgeTwoIdx, long pointTwoIdx, int side)
{

    long tempEdgeIdx;
    tempEdgeIdx = createEdge(pointOneIdx, pointTwoIdx);

    if(side == 0) {
        if(edges[edgeOneIdx].origin == pointOneIdx) {
            addEdgeToRing(edges[edgeOneIdx].originPrev, tempEdgeIdx, pointOneIdx);
        } else {
            addEdgeToRing(edges[edgeOneIdx].destinationPrev, tempEdgeIdx, pointOneIdx);
        }
        addEdgeToRing(edgeTwoIdx, tempEdgeIdx, pointTwoIdx);
    } else {
        addEdgeToRing(edgeOneIdx, tempEdgeIdx, pointOneIdx);
        if(edges[edgeTwoIdx].origin == pointTwoIdx) {
            addEdgeToRing(edges[edgeTwoIdx].originPrev, tempEdgeIdx, pointTwoIdx);
        } else {
            addEdgeToRing(edges[edgeTwoIdx].destinationPrev, tempEdgeIdx, pointTwoIdx);
        }
    }

    return tempEdgeIdx;
}

void findLowerTangent(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *leftLowerEdgeIdx, long *leftLowerOriginIdx, long *rightLowerEdgeIdx, long *rightLowerOriginIdx)
{
    long left, right;
    long leftOriginIdx, leftDestinationIdx, rightOriginIdx, rightDestinationIdx;
    boolean done;

    left = rightSubLeftEdgeIdx;
    right = leftSubRightEdgeIdx;
    leftOriginIdx = s;
    leftDestinationIdx = otherPoint(left, s);
    
    rightOriginIdx = u;
    rightDestinationIdx = otherPoint(right, u);

    done = FALSE;

    while(!done) {
        if(triangleCheck(leftOriginIdx, leftDestinationIdx, rightOriginIdx) > 0.0) {
            left = previousEdge(left, leftDestinationIdx);
            leftOriginIdx = leftDestinationIdx;
            leftDestinationIdx = otherPoint(left, leftOriginIdx);
        } else if(triangleCheck(rightOriginIdx, rightDestinationIdx, leftOriginIdx) < 0.0) {
            right = nextEdge(right, rightDestinationIdx);
            rightOriginIdx = rightDestinationIdx;
            rightDestinationIdx = otherPoint(right, rightOriginIdx);
        } else {
            done = TRUE;
        }
    }

    *leftLowerEdgeIdx = left;
    *rightLowerEdgeIdx = right;
    *leftLowerOriginIdx = leftOriginIdx;
    *rightLowerOriginIdx = rightOriginIdx;
}

void merge(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *lowerCommonTangentIdx)
{
    long base, leftCandidateIdx, rightCandidateIdx;
    long baseOriginIdx, baseDestinationIdx;
    double pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB;
    double pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB;
    double cpLeftCandidate, cpRightCandidate;
    double dpLeftCandidate, dpRightCandidate;

    boolean aboveLeftCandidate, aboveRightCandidate, aboveNext, abovePrev;
    long leftCandidateDestinationIdx, rightCandidateDestinationIdx;
    double cotLeftCandidate, cotRightCandidate;
    long leftLowerEdgeIdx, rightLowerEdgeIdx;
    long rightLowerOriginIdx, leftLowerOriginIdx;

    findLowerTangent(rightSubLeftEdgeIdx, s, leftSubRightEdgeIdx, u, &leftLowerEdgeIdx, &leftLowerOriginIdx, &rightLowerEdgeIdx, &rightLowerOriginIdx);
    base = makeTriangle(leftLowerEdgeIdx, leftLowerOriginIdx, rightLowerEdgeIdx, rightLowerOriginIdx, 1);
    baseOriginIdx = leftLowerOriginIdx;
    baseDestinationIdx = rightLowerOriginIdx;

    *lowerCommonTangentIdx = base;

    do {
        
        leftCandidateIdx = nextEdge(base, baseOriginIdx);
        rightCandidateIdx = previousEdge(base, baseDestinationIdx);

        leftCandidateDestinationIdx = otherPoint(leftCandidateIdx, baseOriginIdx);
        rightCandidateDestinationIdx = otherPoint(rightCandidateIdx, baseDestinationIdx);
        
        makeVector(leftCandidateDestinationIdx, baseOriginIdx, pLeftCandidateOB, qLeftCandidateOB);
        makeVector(leftCandidateDestinationIdx, baseDestinationIdx, pLeftCandidateDB, qLeftCandidateDB);
        makeVector(rightCandidateDestinationIdx, baseOriginIdx, pRightCandidateOB, qRightCandidateOB);
        makeVector(rightCandidateDestinationIdx, baseDestinationIdx, pRightCandidateDB, qRightCandidateDB);

        cpLeftCandidate = crossProduct(pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB);
        cpRightCandidate = crossProduct(pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB);
        
        aboveLeftCandidate = cpLeftCandidate > 0.0;
        aboveRightCandidate = cpRightCandidate > 0.0;

        if(!aboveLeftCandidate && !aboveRightCandidate) {
            break;
        }        

        if(aboveLeftCandidate) {
            double pNextOB, qNextOB, pNextDB, qNextDB;
            double cpNext, dpNext, cotNext;
            long next;
            long nextDestination;

            dpLeftCandidate = dotProduct(pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB);
            cotLeftCandidate = dpLeftCandidate / cpLeftCandidate;

            do {
                next = nextEdge(leftCandidateIdx, baseOriginIdx);
                nextDestination = otherPoint(next, baseOriginIdx);

                makeVector(nextDestination, baseOriginIdx, pNextOB, qNextOB);
                makeVector(nextDestination, baseDestinationIdx, pNextDB, qNextDB);

                cpNext = crossProduct(pNextOB, qNextOB, pNextDB, qNextDB);
                aboveNext = cpNext > 0.0;

                if(!aboveNext) {
                    break;
                }

                dpNext = dotProduct(pNextOB, qNextOB, pNextDB, qNextDB);
                cotNext = dpNext / cpNext;
                
                if(cotNext > cotLeftCandidate) {
                    break;
                }
                
                deleteEdge(leftCandidateIdx);
                leftCandidateIdx = next;
                cotLeftCandidate = cotNext;
            } while(TRUE);

        }

        if(aboveRightCandidate) {
        
            double pPrevOB, qPrevOB, pPrevDB, qPrevDB;
            double cpPrev, dpPrev, cotPrev;
            long prev;
            long prevDestination;

            dpRightCandidate = dotProduct(pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB);
            cotRightCandidate = dpRightCandidate / cpRightCandidate;            

            do
            {
                
                prev = previousEdge(rightCandidateIdx, baseDestinationIdx);
                prevDestination = otherPoint(prev, baseDestinationIdx);
                
                makeVector(prevDestination, baseOriginIdx, pPrevOB, qPrevOB);
                makeVector(prevDestination, baseDestinationIdx, pPrevDB, qPrevDB);

                cpPrev = crossProduct(pPrevOB, qPrevOB, pPrevDB, qPrevDB);
                abovePrev = cpPrev > 0.0;

                if(!abovePrev) {
                    break;
                }
    
                dpPrev = dotProduct(pPrevOB, qPrevOB, pPrevDB, qPrevDB);
                cotPrev = dpPrev / cpPrev;

                if(cotPrev > cotRightCandidate) {
                    break;
                }

                deleteEdge(rightCandidateIdx);
                rightCandidateIdx = prev;
                cotRightCandidate = cotPrev;
                
            } while(TRUE);

        }            

        leftCandidateDestinationIdx = otherPoint(leftCandidateIdx, baseOriginIdx);
        rightCandidateDestinationIdx = otherPoint(rightCandidateIdx, baseDestinationIdx);
        if(!aboveLeftCandidate || (aboveLeftCandidate && aboveRightCandidate && cotRightCandidate < cotLeftCandidate) ) {
            base = makeTriangle(base, baseOriginIdx, rightCandidateIdx, rightCandidateDestinationIdx, 1); // side = right
            baseDestinationIdx = rightCandidateDestinationIdx;
        } else {
            base = makeTriangle(leftCandidateIdx, leftCandidateDestinationIdx, base, baseDestinationIdx, 1); // side = right
            baseOriginIdx = leftCandidateDestinationIdx;
        }
        
    } while(TRUE);    
    
}

void deleteEdge(long e)
{
    long p, q;

    p = edges[e].origin;
    q = edges[e].destination;

    if(points[p].entryPoint == e) {
        points[p].entryPoint = edges[e].originNext;
    }
    if(points[q].entryPoint == e) {
        points[q].entryPoint = edges[e].destinationNext;
    }

    if(edges[edges[e].originNext].origin == p) {
        edges[edges[e].originNext].originPrev = edges[e].originPrev;
    } else {
        edges[edges[e].originNext].destinationPrev = edges[e].originPrev;
    }

    if(edges[edges[e].originPrev].origin == p) {
        edges[edges[e].originPrev].originNext = edges[e].originNext;
    } else {
        edges[edges[e].originPrev].destinationNext = edges[e].originNext;
    }

    if(edges[edges[e].destinationNext].origin == q) {
        edges[edges[e].destinationNext].originPrev = edges[e].destinationPrev;
    } else {
        edges[edges[e].destinationNext].destinationPrev = edges[e].destinationPrev;
    }

    if(edges[edges[e].destinationPrev].origin == q) {
        edges[edges[e].destinationPrev].originNext = edges[e].destinationNext;
    } else {
        edges[edges[e].destinationPrev].destinationNext = edges[e].destinationNext;
    }

    

    edges[e].origin = -1;
    edges[e].destination = -1;
    edges[e].originNext = -1;
    edges[e].destinationNext = -1;
    edges[e].originPrev = -1;
    edges[e].destinationPrev = -1;

}

void printEdges(long myStart, long myEnd)
{
    long eStart, e;
    Point *u, *v;
    long i;
    long temp;

    for(i=myStart;i<myEnd;i++) {
        u = &points[i];
        eStart = e = points[i].entryPoint;
        do {
            temp = otherPoint(e,i);

            v = &points[temp];
            if(u < v) {
                printf("%ld %ld\n", u-points, v-points);
            }
            e = nextEdge(e, i);
        } while(e != eStart);
    }       
}

void writeEdges(const char* filename, long myStart, long myEnd)
{
    FILE* f = fopen(filename, "w");
   
    if(f == NULL) {
        logger(ERROR, "Failed to write to file");
        exit(1);
    }

    long eStart, e;
    Point *u, *v;
    long i;
    long temp;

    for(i=myStart;i<myEnd;i++) {
        u = &points[i];
        eStart = e = points[i].entryPoint;
        do {
            temp = otherPoint(e,i);

            v = &points[temp];
            if(u < v) {
                fprintf(f, "%ld %ld\n", u-points, v-points);
            }
            e = nextEdge(e, i);
        } while(e != eStart);
    }
    fclose(f);
}


void printEdge(long e)
{
    printf("[origin:%ld; destination:%ld; originNext:%ld; originPrev:%ld; destinationNext:%ld; destinationPrev:%ld]\n",
            edges[e].origin, edges[e].destination, edges[e].originNext, edges[e].originPrev, edges[e].destinationNext, edges[e].destinationPrev);
}

void printPoint(long p)
{
    printf("[x:%lf, y=%lf, entryPoint:%ld]\n", points[p].x, points[p].y, points[p].entryPoint);
}
