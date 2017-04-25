#include <stdio.h>
#include <stdlib.h>
#include "structuress.h"

#define COUNT 5

void delaunay(Point *points, int start, int end, Edge **leftEdge, Edge **rightEdge);
Edge* createEdge(Point *start, Point *end);
void deleteEdge(Edge *e);
void addEdgeToRing(Edge *edgeOne, Edge *edgeTwo, Point *commonPoint);
int triangleCheck(Point *one, Point *two, Point *three);
Edge* makeTriangle(Edge *edgeOne, Point *pointOne, Edge *edgeTwo, Point *pointTwo, int side);
static void findLowerTangent(Edge *rightSubLeftEdge, Point *s, Edge *leftSubRightEdge, Point *u, Edge **leftLowerEdge, Point **leftLowerOrigin, Edge **rightLowerEdge, Point **rightLowerOrigin);
static void merge(Edge *rightSubLeftEdge, Point *s, Edge *leftSubRightEdge, Point *u, Edge **lowerCommonTangent);
void printEdges(Point *points);
void printEdge(Edge *edge);
void printPoint(Point *p);

int main(int argc, char *argv[])
{

    Edge *leftEdge, *rightEdge;
    if(argc != 2)
    {
        printf("Usage: hull <data_file_as csv>\n");
        exit(1);
    }

    char *inputFileName = argv[1];
    
    int temp, i = 0;

    FILE *inputFile = fopen(inputFileName, "r");
    
    Point *points = malloc(sizeof(Point) * COUNT);
    if(points == NULL)
    {
        printf("Failed to allocate memory!\n");
        exit(3);
    }

    while(fscanf(inputFile, "%d,%f,%f", &temp, &points[i].x, &points[i].y) == 3)
    {
        i++;
    }
    fclose(inputFile);

    printf("%d points read into memory from file.\n", i);

/*
    for(i=0; i<COUNT; i++)
    {
        printf("(%f, %f)\n", points[i].x, points[i].y);
    }
*/
    delaunay(points, 0, COUNT-1, &leftEdge, &rightEdge);
    printEdges(points);

    return 0;
}

void delaunay(Point *points, int start, int end, Edge **leftEdge, Edge **rightEdge)
{
    //printf("Start: %d; End: %d\n", start, end);

    Edge *leftSubLeftEdge, *leftSubRightEdge, *rightSubLeftEdge, *rightSubRightEdge;
    Edge *lowerCommonTangent;
    Edge *edgeOne, *edgeTwo, *edgeThree;

    int pointCount = end - start + 1;
    
    if(pointCount == 2)
    {
        //printf("pointCount = %d\n", pointCount);
        *leftEdge = *rightEdge = createEdge(&points[start], &points[end]);
        //printf("Edge created: (%f,%f)---->(%d,%d)\n", (*leftEdge)->origin->x, (*leftEdge)->origin->y, (*leftEdge)->destination->x, (*leftEdge)->destination->y);
        return;
    } else if(pointCount == 3) {
        
        edgeOne = createEdge(&points[start], &points[start+1]);
        edgeTwo = createEdge(&points[start+1], &points[end]);
        addEdgeToRing(edgeOne, edgeTwo, &points[start+1]);
        float direction = triangleCheck(&points[start], &points[start+1], &points[end]);
        
        if(direction > 0.0) {
                printf("Right\n");
            edgeThree = makeTriangle(edgeOne, &points[start], edgeTwo, &points[end], 1); //side = right
            *leftEdge = edgeOne;
            *rightEdge = edgeTwo;
        } else if(direction < 0.0) {
            edgeThree = makeTriangle(edgeOne, &points[start], edgeTwo, &points[end], 0); //side = left
            *leftEdge = edgeThree;
            *rightEdge = edgeThree;
        } else { 
            *leftEdge = edgeOne;
            *rightEdge = edgeTwo;
        }

    } else if(pointCount > 3) {
        int mid = start + ((end - start)/2);

        delaunay(points, start, mid, &leftSubLeftEdge, &rightSubLeftEdge);
        delaunay(points, mid+1, end, &leftSubRightEdge, &rightSubRightEdge);

        merge(rightSubLeftEdge, &points[mid], leftSubRightEdge, &points[mid+1], &lowerCommonTangent);

        if(lowerCommonTangent->origin == &points[start]) {
            leftSubLeftEdge = lowerCommonTangent;
        } 
        
        if(lowerCommonTangent->destination == &points[end]) {
            rightSubRightEdge = lowerCommonTangent;
        }

        *leftEdge = leftSubLeftEdge;
        *rightEdge = rightSubRightEdge;
            
    }


}

Edge* createEdge(Point *start, Point *end)
{
    Edge *e;
    e = malloc(sizeof(Edge));
    e->origin = start;
    e->destination = end;
    
    
    e->originNext = e->originPrev = e->destinationNext = e->destinationPrev = e;
    
    if(start->entryPoint == NULL)
    {
        start->entryPoint = e;
    }
    if(end->entryPoint == NULL)
    {
        end->entryPoint = e;
    }
    //printf("Edge created: (%f,%f)---->(%f,%f)\n", start->x, start->y, end->x, end->y);
    return e;
}

void deleteEdge(Edge *e)
{
    Point *p, *q;
    
    //printf("Edge deleted: (%f,%f)---->(%f,%f)\n", e->origin->x, e->origin->y, e->destination->x, e->destination->y);
    p = e->origin;
    q = e->destination;

    if(p->entryPoint == e) {
        p->entryPoint = e->originNext;
    }
    if(q->entryPoint == e) {
        q->entryPoint = e->destinationNext;
    }

    if(e->originNext->origin == p) {
        e->originNext->originPrev = e->originPrev;
    } else {
        e->originNext->destinationPrev = e->originPrev;
    }

    if(e->originPrev->origin == p) {
        e->originPrev->originNext = e->originNext;
    } else {
        e->originPrev->destinationNext = e->originNext;
    }

    if(e->destinationNext->origin == q) {
        e->destinationNext->originPrev = e->destinationPrev;
    } else {
        e->destinationNext->destinationPrev = e->destinationPrev;
    }

    if(e->destinationPrev->origin == q) {
        e->destinationPrev->originNext = e->destinationNext;
    } else {
        e->destinationPrev->destinationNext = e->destinationNext;
    }

    free(e);    
}

void addEdgeToRing(Edge *edgeOne, Edge *edgeTwo, Point *commonPoint)
{
    Edge *tempEdge;


    if(edgeOne->origin == commonPoint) {
        tempEdge = edgeOne->originNext;
        edgeOne->originNext = edgeTwo;
    } else {
        tempEdge = edgeOne->destinationNext;
        edgeOne->destinationNext = edgeTwo;
    }
    if(tempEdge->origin == commonPoint) {
        tempEdge->originPrev = edgeTwo;
    } else {
        tempEdge->destinationPrev = edgeTwo;
    }
    if(edgeTwo->origin == commonPoint) {
        edgeTwo->originNext = tempEdge;
        edgeTwo->originPrev = edgeOne;
    } else {
        edgeTwo->destinationNext = tempEdge;
        edgeTwo->destinationPrev = edgeOne;
    }
}

int triangleCheck(Point *one, Point *two, Point *three)
{

    return ( (two->x - one->x) * (three->y - one->y) - (two->y - one->y) * (three->x - one->x) );

}

Edge* makeTriangle(Edge *edgeOne, Point *pointOne, Edge *edgeTwo, Point *pointTwo, int side)
{

    Edge *tempEdge;
    
    tempEdge = createEdge(pointOne, pointTwo);
    
    if(side == 0) { //if side == left

        if(edgeOne->origin == pointOne) {
            addEdgeToRing(edgeOne->originPrev, tempEdge, pointOne);
        } else {
            addEdgeToRing(edgeOne->destinationPrev, tempEdge, pointOne);
        }
        addEdgeToRing(edgeTwo, tempEdge, pointTwo);
    } else { //if side == right
        addEdgeToRing(edgeOne, tempEdge, pointOne);
        if(edgeTwo->origin == pointTwo) {
            addEdgeToRing(edgeTwo->originPrev, tempEdge, pointTwo);
        } else {
            addEdgeToRing(edgeTwo->destinationPrev, tempEdge, pointTwo);
        }
    }

    return tempEdge;
}


static void findLowerTangent(Edge *rightSubLeftEdge, Point *s, Edge *leftSubRightEdge, Point *u, Edge **leftLowerEdge, Point **leftLowerOrigin, Edge **rightLowerEdge, Point **rightLowerOrigin)
{

    Edge *left, *right;
    Point *leftOrigin, *leftDestination, *rightOrigin, *rightDestination;
    boolean done;

    left = rightSubLeftEdge;
    right = leftSubRightEdge;
    leftOrigin = s;
    leftDestination = otherPoint(left, s);
    
    rightOrigin = u;
    rightDestination = otherPoint(right, u);


    done = FALSE;

    while(!done) {
        if(triangleCheck(leftOrigin, leftDestination, rightOrigin) > 0.0 ) {
            left = previousEdge(left, leftDestination);
            leftOrigin = leftDestination;
            leftDestination = otherPoint(left, leftOrigin);
        } else if(triangleCheck(rightOrigin, rightDestination, leftOrigin) < 0.0 ) {
            right = nextEdge(right, rightDestination);
            rightOrigin = rightDestination;
            rightDestination = otherPoint(right, rightOrigin);
        } else {
            done = TRUE;
        }
    }
    
    *leftLowerEdge = left;
    *rightLowerEdge = right;
    *leftLowerOrigin = leftOrigin;
    *rightLowerOrigin = rightOrigin;

}

static void merge(Edge *rightSubLeftEdge, Point *s, Edge *leftSubRightEdge, Point *u, Edge **lowerCommonTangent)
{
    Edge *base, *leftCandidate, *rightCandidate;
    Point *baseOrigin, *baseDestination;
    float pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB;
    float pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB;
    float cpLeftCandidate, cpRightCandidate;
    float dpLeftCandidate, dpRightCandidate;

    boolean aboveLeftCandidate, aboveRightCandidate, aboveNext, abovePrev;
    Point *leftCandidateDestination, *rightCandidateDestination;
    float cotLeftCandidate, cotRightCandidate;
    Edge *leftLowerEdge, *rightLowerEdge;
    Point *rightLowerOrigin, *leftLowerOrigin;

    findLowerTangent(rightSubLeftEdge, s, leftSubRightEdge, u, &leftLowerEdge, &leftLowerOrigin, &rightLowerEdge, &rightLowerOrigin);
    base = makeTriangle(leftLowerEdge, leftLowerOrigin, rightLowerEdge, rightLowerOrigin, 1); //side = right
    baseOrigin = leftLowerOrigin;
    baseDestination = rightLowerOrigin;

    *lowerCommonTangent = base;

    do
    {
        
        leftCandidate = nextEdge(base, baseOrigin);
        rightCandidate = previousEdge(base, baseDestination);

        leftCandidateDestination = otherPoint(leftCandidate, baseOrigin);
        rightCandidateDestination = otherPoint(rightCandidate,baseDestination);

        makeVector(leftCandidateDestination, baseOrigin, pLeftCandidateOB, qLeftCandidateOB);
        makeVector(leftCandidateDestination, baseDestination, pLeftCandidateDB, qLeftCandidateDB);
        makeVector(rightCandidateDestination, baseOrigin, pRightCandidateOB, qRightCandidateOB);
        makeVector(rightCandidateDestination, baseDestination, pRightCandidateDB, qRightCandidateDB);

        cpLeftCandidate = crossProduct(pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB);
        cpRightCandidate = crossProduct(pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB);

        aboveLeftCandidate = cpLeftCandidate > 0.0;
        aboveRightCandidate = cpRightCandidate > 0.0;

        if(!aboveLeftCandidate && !aboveRightCandidate) {
            break;
        }

        if(aboveLeftCandidate) {
            
            float pNextOB, qNextOB, pNextDB, qNextDB;
            float cpNext, dpNext, cotNext;
            Edge *next;
            Point *nextDestination;

            dpLeftCandidate = dotProduct(pLeftCandidateOB, qLeftCandidateOB, pLeftCandidateDB, qLeftCandidateDB);
            cotLeftCandidate = dpLeftCandidate / cpLeftCandidate;

            do
            {
                
                next = nextEdge(leftCandidate, baseOrigin);
                nextDestination = otherPoint(next, baseOrigin);

                makeVector(nextDestination, baseOrigin, pNextOB, qNextOB);
                makeVector(nextDestination, baseDestination, pNextDB, qNextDB);

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
                
                deleteEdge(leftCandidate);
                leftCandidate = next;
                cotLeftCandidate = cotNext;
                
            } while(TRUE);
                        
            
        }
        
        if(aboveRightCandidate) {
        
            float pPrevOB, qPrevOB, pPrevDB, qPrevDB;
            float cpPrev, dpPrev, cotPrev;
            Edge *prev;
            Point *prevDestination;

            dpRightCandidate = dotProduct(pRightCandidateOB, qRightCandidateOB, pRightCandidateDB, qRightCandidateDB);
            cotRightCandidate = dpRightCandidate / cpRightCandidate;            

            do
            {
                
                prev = previousEdge(rightCandidate, baseDestination);
                prevDestination = otherPoint(prev, baseDestination);
                
                makeVector(prevDestination, baseOrigin, pPrevOB, qPrevOB);
                makeVector(prevDestination, baseDestination, pPrevDB, qPrevDB);

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

                deleteEdge(rightCandidate);
                rightCandidate = prev;
                cotRightCandidate = cotPrev;
                
            } while(TRUE);

        }

        leftCandidateDestination = otherPoint(leftCandidate, baseOrigin);
        rightCandidateDestination = otherPoint(rightCandidate, baseDestination);
        if(!aboveLeftCandidate || (aboveLeftCandidate && aboveRightCandidate && cotRightCandidate < cotLeftCandidate) ) {
            base = makeTriangle(base, baseOrigin, rightCandidate, rightCandidateDestination, 1); // side = right
            baseDestination = rightCandidateDestination;
        } else {
            base = makeTriangle(leftCandidate, leftCandidateDestination, base, baseDestination, 1); // side = right
            baseOrigin = leftCandidateDestination;
        }

    } while(TRUE);
}


void printEdges(Point *points)
{

    Edge *eStart, *e;
    Point *u, *v;
    int i;

    for(i=0;i<COUNT;i++) {
        u = &points[i];
        eStart = e = u->entryPoint;
        do
        {
            v = otherPoint(e, u);
            if(u < v) {
                printf("%ld %ld\n", u - points, v - points);
            }
            e = nextEdge(e, u);
        } while(e != eStart);
    }
}

void printEdge(Edge *e)
{
    printf("[origin:(%f,%f); destination:(%f,%f), oNext:(%f,%f)->(%f,%f), oPrev:(%f,%f)->(%f,%f)]\n",e->origin->x,
        e->origin->y, e->destination->x, e->destination->y, e->originNext->origin->x, e->originNext->origin->y,
        e->originNext->destination->x, e->originNext->destination->y, e->originPrev->origin->x, e->originPrev->origin->y,
        e->originPrev->destination->x, e->originPrev->destination->y);
}

void printPoint(Point *p)
{
    printf("(%f,%f)\n", p->x, p->y);
}
