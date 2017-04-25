

#define  otherPoint(e,p)  (edges[e].origin == p ? edges[e].destination : edges[e].origin)
#define  previousEdge(e,p)  (edges[e].origin == p ? edges[e].originPrev : edges[e].destinationPrev)
#define  nextEdge(e,p)  (edges[e].origin == p ? edges[e].originNext : edges[e].destinationNext)
#define  makeVector(p1,p2,u,v) (u = points[p2].x - points[p1].x, v = points[p2].y - points[p1].y)
#define  crossProduct(u1,v1,u2,v2) (u1 * v2 - v1 * u2)
#define  dotProduct(u1,v1,u2,v2) (u1 * u2 + v1 * v2)

#define TRUE 1
#define FALSE 0

typedef struct Point Point;
typedef struct Edge Edge;
typedef unsigned int boolean;

struct Point
{
    float x;
    float y;
    long entryPoint;
};

struct Edge
{
    long origin;
    long destination;

    long originNext;
    long originPrev;
    long destinationNext;
    long destinationPrev;
};
