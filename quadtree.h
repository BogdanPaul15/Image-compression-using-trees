#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PIXEL_SIZE 3
#define MAX_FORMAT_SIZE 3
#define MAX_Q_COLOR 255

/* defining the structure of a pixel in RGB (red, green, blue) format */

typedef struct pixel {
    unsigned char red, green, blue;
} RGB;

/* defining the quadtree structure, where node1 -> top left,
 * node2 -> top right, node3 -> bottom right, node4 -> bottom left with
 * pixel based info in RGB (red, green, blue) format */

typedef struct nod {
    RGB info;
    int type;
    struct nod *node1, *node2, *node3, *node4;
} TNode, *TArb;

/* defining a simple linked list with info type
 * quadtree */

typedef struct cell {
    TArb arb;
    struct cell *next;
} TCell, *TList;

/* defining the queue structure */

typedef struct queue {
    TList start, end;
} TQueue;

// image functions

RGB **allocMatrix(unsigned int size);
void createImage(FILE *out, TArb arb, unsigned int size);
RGB **prepareMatrix(FILE *in, FILE *out, unsigned int *height,
                    unsigned int *width, const char *fact, int *factor);
void imageReshape(RGB **pic, TArb arb, int size, int start_x, int start_y,
                  int end_x, int end_y);

// quadtree functions

TArb createQuadtree(FILE *in, unsigned int *size);
TArb divideQuadtree(RGB **pic, TArb arb, int start_x, int start_y, int end_x,
                    int end_y, int size, int factor);
void breadthFirst(FILE *out, TArb arb);
TArb constrNodeInt();
TArb updateLeaf(TArb arb, unsigned char r, unsigned char g, unsigned char b);
TArb createLeaf(unsigned char r, unsigned char g, unsigned char b);
int numLeafs(TArb arb);
int numLevels(TArb arb);
int getNearestLeafLevel(TArb arb);
void destroyTree(TArb arb);

// queue functions

TQueue *initQ();
int intrQ(TQueue *queue, TArb arb);
TArb extrQ(TQueue *queue, TArb arb);
int verifQNull(TQueue *queue);
void resetQ(TQueue *queue);
void destroyList(TList *aL);
void destrQ(TQueue **queue);

// other

void openFiles(FILE **in, FILE **out, const char *inFileName,
               const char *outFileName);
void solveC1(FILE *out, TArb arb, unsigned int height);