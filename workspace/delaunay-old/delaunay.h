void delaunay(long start, long end, long *leftEdgeIdx, long *rightEdgeIdx);
long createEdge(long startPointIdx, long endPointIdx);
void addEdgeToRing(long edgeOneIdx, long edgeTwoIdx, long commonPointIdx);
double triangleCheck(long pointOneIdx, long pointTwoIdx, long pointThreeIdx);
long makeTriangle(long edgeOneIdx, long pointOneIdx, long edgeTwoIdx, long pointTwoIdx, int side);
void merge(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *lowerCommonTangentIdx);
void findLowerTangent(long rightSubLeftEdgeIdx, long s, long leftSubRightEdgeIdx, long u, long *leftLowerEdgeIdx, long *lowerLowerOriginIdx, long *rightLowerEdgeIdx, long *rightLowerOriginIdx);

void printEdges(long myStart, long myEnd);
void writeEdges(const char* filename, long myStart, long myEnd);
void printEdge();
void printPoint(long p);
void deleteEdge(long e);

