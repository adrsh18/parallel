

#define  otherPoint(e,p)  ((e)->origin == p ? (e)->destination : (e)->origin)
#define  previousEdge(e,p)  ((e)->origin == p ? (e)->originPrev : (e)->destinationPrev)
#define  nextEdge(e,p)  ((e)->origin == p ? (e)->originNext : (e)->destinationNext)
#define  makeVector(p1,p2,u,v) (u = p2->x - p1->x, v = p2->y - p1->y)
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
    Edge *entryPoint;
};

struct Edge
{
    Point *origin;
    Point *destination;

    Edge *originNext;
    Edge *originPrev;
    Edge *destinationNext;
    Edge *destinationPrev;
};
