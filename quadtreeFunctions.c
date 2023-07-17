#include "quadtree.h"

// IMAGE FUNCTIONS

RGB **allocMatrix(unsigned int size) {
    int i = 0;
    // allocates a matrix of pixels and returns it
    RGB **pic = (RGB **)malloc(size * sizeof(RGB *));
    if (!pic) return NULL;
    for (i = 0; i < size; i++) {
        pic[i] = (RGB *)malloc(size * sizeof(RGB));
        if (!pic[i]) return NULL;
    }
    return pic;
}

void createImage(FILE *out, TArb arb, unsigned int size) {
    int i = 0, j = 0;
    RGB **pic = allocMatrix(size);
    char format[] = "P6\n";
    /*  it is written in the binary file the format, width and height and
     *  the max amount of color (.PPM format)
     */
    fprintf(out, "%s", format);
    fprintf(out, "%d %d\n", size, size);
    fprintf(out, "%d\n", MAX_Q_COLOR);
    // starts image reshape by the quadtree built earlier
    imageReshape(pic, arb, size, 0, 0, size, size);
    /*  write all pixels in order (red, green, blue) on the same row
     *  and without any whitespace between them
     */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fwrite(&pic[i][j].red, sizeof(unsigned char), 1, out);
            fwrite(&pic[i][j].green, sizeof(unsigned char), 1, out);
            fwrite(&pic[i][j].blue, sizeof(unsigned char), 1, out);
        }
    }
    for (i = 0; i < size; i++) {  // free the pixel matrix
        free(pic[i]);
    }
    free(pic);
}

RGB **prepareMatrix(FILE *in, FILE *out, unsigned int *height,
                    unsigned int *width, const char *fact, int *factor) {
    int i = 0, j = 0;
    char format[MAX_FORMAT_SIZE];
    unsigned int max = 0;
    fread(format, sizeof(format), 1, in);  // reading file format ("P6")
    fscanf(in, "%d %d %d", &(*width), &(*height), &max);
    RGB **pic = allocMatrix(*height);
    getc(in);  // reading endline
    unsigned char RGB[MAX_PIXEL_SIZE];
    /*  reading tree (red, green, blue) unsigned chars from the file and fill
     *  with corresponding pixel from the picture matrix
     */
    for (i = 0; i < *height; i++) {
        for (j = 0; j < *width; j++) {
            fread(RGB, sizeof(unsigned char), 3, in);
            pic[i][j].red = RGB[0];
            pic[i][j].green = RGB[1];
            pic[i][j].blue = RGB[2];
        }
    }
    *factor = atoi(fact);  // reading the factor and convert it into integer
    return pic;
}

void imageReshape(RGB **pic, TArb arb, int size, int startX, int startY,
                  int endX, int endY) {
    /*  rebuilds the pixel matrix of the picture based on the received quadtree,
     *  the start point, the end point and the dimension
     */
    int i = 0, j = 0;
    if (arb->type == 1) {
        /*  if current node is external (leaf), then
         *  write to the dedicated portion of the array the values
         *  RGB (red, green, blue) of the leaf
         */
        for (i = startX; i < startX + size; i++) {
            for (j = startY; j < startY + size; j++) {
                pic[i][j].red = arb->info.red;
                pic[i][j].green = arb->info.green;
                pic[i][j].blue = arb->info.blue;
            }
        }
    } else {
        /*  if the current node is internal, then with the square coordinates
         *  and halved dimension it is formed a new block of pixels in the
         *  matrix, for every child of the current node
         */
        imageReshape(pic, arb->node1, size / 2, startX, startY,
                     (startX + endX) / 2, (startY + endY) / 2);
        imageReshape(pic, arb->node2, size / 2, startX, (startY + endY) / 2,
                     (startX + endX) / 2, endY);
        imageReshape(pic, arb->node3, size / 2, startX + (endX - startX) / 2,
                     startY + (endY - startY) / 2, endX, endY);
        imageReshape(pic, arb->node4, size / 2, startX + (endX - startX) / 2,
                     startY, endX, startY + (endY - startY) / 2);
    }
}

// QUADTREE FUNCTIONS

TArb createQuadtree(FILE *in, unsigned int *size) {
    unsigned char type = 0, pixel[MAX_PIXEL_SIZE];
    fread(&(*size), sizeof(unsigned int), 1, in);  // reads size of image
    TArb arb = constrNodeInt();  // create and allocate a quadtree
    fread(&type, sizeof(unsigned char), 1, in);
    /*  read the node type, if it is 1 (leaf), then the quadtree has 1 node, so
     *  the image has only 1 color (reads the 3 values red, green, blue), else,
     *  insert the node into the queue and verify each child
     */
    if (type == 0) {
        TQueue *q = initQ();
        intrQ(q, arb);
        TArb node = NULL;
        while (!verifQNull(q)) {
            node = extrQ(q, node);
            if (verifQNull(q) == 1) resetQ(q);
            if (node->type == 0) {
                fread(&type, sizeof(unsigned char), 1, in);
                if (type == 0) {
                    node->node1 = constrNodeInt();
                } else if (type == 1) {
                    fread(pixel, sizeof(unsigned char), 3, in);
                    node->node1 = createLeaf(pixel[0], pixel[1], pixel[2]);
                }
                intrQ(q, node->node1);
                fread(&type, sizeof(unsigned char), 1, in);
                if (type == 0) {
                    node->node2 = constrNodeInt();
                } else if (type == 1) {
                    fread(pixel, sizeof(unsigned char), 3, in);
                    node->node2 = createLeaf(pixel[0], pixel[1], pixel[2]);
                }
                intrQ(q, node->node2);
                fread(&type, sizeof(unsigned char), 1, in);
                if (type == 0) {
                    node->node3 = constrNodeInt();
                } else if (type == 1) {
                    fread(pixel, sizeof(unsigned char), 3, in);
                    node->node3 = createLeaf(pixel[0], pixel[1], pixel[2]);
                }
                intrQ(q, node->node3);
                fread(&type, sizeof(unsigned char), 1, in);
                if (type == 0) {
                    node->node4 = constrNodeInt();
                } else if (type == 1) {
                    fread(pixel, sizeof(unsigned char), 3, in);
                    node->node4 = createLeaf(pixel[0], pixel[1], pixel[2]);
                }
                intrQ(q, node->node4);
            }
        }
        destrQ(&q);
    } else {
        fread(pixel, sizeof(unsigned char), 3, in);
        arb = updateLeaf(arb, pixel[0], pixel[1], pixel[2]);
    }
    return arb;
}

TArb divideQuadtree(RGB **pic, TArb arb, int startX, int startY, int endX,
                    int endY, int size, int factor) {
    /*  creates and splits the quadtree based on the given matrix
     *  (receives start point and end point, dimensions and division factor)
     */
    unsigned long long sumR = 0, sumG = 0, sumB = 0, red = 0, blue = 0,
                       green = 0;
    int i = 0, j = 0;
    // perform the sums according to the formulas
    for (i = startX; i < startX + size; i++) {
        for (j = startY; j < startY + size; j++) {
            sumR = sumR + pic[i][j].red;
            sumG = sumG + pic[i][j].green;
            sumB = sumB + pic[i][j].blue;
        }
    }
    red = sumR / (size * size);
    green = sumG / (size * size);
    blue = sumB / (size * size);
    unsigned long long sum = 0, mean = 0;
    for (i = startX; i < startX + size; i++) {
        for (j = startY; j < startY + size; j++) {
            sum = sum + (red - pic[i][j].red) * (red - pic[i][j].red) +
                  (green - pic[i][j].green) * (green - pic[i][j].green) +
                  (blue - pic[i][j].blue) * (blue - pic[i][j].blue);
        }
    }
    // the similarity score is calculated according to the formula
    mean = sum / (3 * (size * size));
    /*  if the similarity score is greater than the factor, then it is called
     *  recursively, splitting the tree into 4 equal squares
     */
    if (mean > factor) {
        arb->node1 = constrNodeInt();  // allocating memory for an internal node
        /*  calls the same function, but with half of its
         *  current dimension and with the new square coordinates
         *  for every node (4)
         */
        arb->node1 =
            divideQuadtree(pic, arb->node1, startX, startY, (startX + endX) / 2,
                           (startY + endY) / 2, size / 2, factor);
        arb->node2 = constrNodeInt();
        arb->node2 =
            divideQuadtree(pic, arb->node2, startX, (startY + endY) / 2,
                           (startX + endX) / 2, endY, size / 2, factor);
        arb->node3 = constrNodeInt();
        arb->node3 = divideQuadtree(
            pic, arb->node3, startX + (endX - startX) / 2,
            startY + (endY - startY) / 2, endX, endY, size / 2, factor);
        arb->node4 = constrNodeInt();
        arb->node4 = divideQuadtree(
            pic, arb->node4, startX + (endX - startX) / 2, startY, endX,
            startY + (endY - startY) / 2, size / 2, factor);
    } else {
        /*  if the similarity score is not greater than the factor, then just
         *  update the initially allocated node as internal to external (leaf)
         *  and fill in the RGB values ​​(red, green, blue)
         */
        arb = updateLeaf(arb, red, green, blue);
    }
    return arb;  // return the split quadtree
}

void breadthFirst(FILE *out, TArb arb) {
    TArb nod;
    unsigned int c = 0;
    TQueue *q = initQ();  // initialize queue
    intrQ(q, arb);        // inserts into queue the root of the quadtree

    /*  while the queue is not empty, extract the first node in the queue
     *  if it is internal write the node type, else write the node type and
     *  specific info red, green, blue values
     *  verify each child of the current node and insert them into the queue
     */
    while (!verifQNull(q)) {
        nod = extrQ(q, nod);
        if (verifQNull(q) == 1) resetQ(q);
        if (nod->type == 0) {
            c = nod->type;
            fwrite(&c, sizeof(unsigned char), 1, out);
        } else {
            c = nod->type;
            fwrite(&c, sizeof(unsigned char), 1, out);
            fwrite(&nod->info.red, sizeof(unsigned char), 1, out);
            fwrite(&nod->info.green, sizeof(unsigned char), 1, out);
            fwrite(&nod->info.blue, sizeof(unsigned char), 1, out);
        }
        if (nod->node1 != NULL) intrQ(q, nod->node1);
        if (nod->node2 != NULL) intrQ(q, nod->node2);
        if (nod->node3 != NULL) intrQ(q, nod->node3);
        if (nod->node4 != NULL) intrQ(q, nod->node4);
    }
    destrQ(&q);  // destroy and free the queue
}

TArb constrNodeInt() {
    /*  creates and allocate an internal type node in which all descendants are
     *  null, so we can change them afterwards and re-allocate
     *  ! internal nodes don't have RGB (red, green, blue) info
     */
    TArb aux = (TArb)malloc(sizeof(TNode));
    if (!aux) return NULL;
    aux->node1 = NULL;
    aux->node2 = NULL;
    aux->node3 = NULL;
    aux->node4 = NULL;
    aux->type = 0;  // type of this node -> internal
    return aux;
}

TArb updateLeaf(TArb aux, unsigned char r, unsigned char g, unsigned char b) {
    /*  updates a node already allocated and turns it into an external
     *  type node (leaf) by filling in with specific info (red, green, blue)
     */
    if (!aux) return NULL;
    aux->info.red = r;
    aux->info.green = g;
    aux->info.blue = b;
    aux->node1 = NULL;
    aux->node2 = NULL;
    aux->node3 = NULL;
    aux->node4 = NULL;
    aux->type = 1;  // type of this node -> external
    return aux;
}

TArb createLeaf(unsigned char r, unsigned char g, unsigned char b) {
    /*  creates an external type node (leaf)
     *  in which all descendants are null and complete
     *  with specific info (red, green, blue)
     */
    TArb arb = (TArb)malloc(sizeof(TNode));
    if (!arb) return NULL;
    arb->info.red = r;
    arb->info.green = g;
    arb->info.blue = b;
    arb->node1 = NULL;
    arb->node2 = NULL;
    arb->node3 = NULL;
    arb->node4 = NULL;
    arb->type = 1;  // type of this node -> external
    return arb;
}

int numLeafs(TArb arb) {
    if (!arb) return 0;
    /*  computes the number of leaves of each child of the quadtree
     *  and return sum + 1 (root)
     */
    if (arb->type == 1) return 1;
    int leaves = 0;
    leaves = leaves + numLeafs(arb->node1) + numLeafs(arb->node2) +
             numLeafs(arb->node3) + numLeafs(arb->node4);
    return leaves;
}

int numLevels(TArb arb) {
    int n1, n2, n3, n4, maxFT = 0, maxST = 0;
    if (!arb) return 0;
    n1 = numLevels(arb->node1);
    /*  compute the number of levels recursively
     *  on each child, and at the end it is done
     *  the maximum of them, representing the number of
     *  quaternary tree levels (FT -> FIRST
     *  TWO, ST -> SECOND TWO)
     */
    n2 = numLevels(arb->node2);
    n3 = numLevels(arb->node3);
    n4 = numLevels(arb->node4);
    if (n1 >= n2)
        maxFT = n1;
    else
        maxFT = n2;
    if (n3 >= n4)
        maxST = n3;
    else
        maxST = n4;
    return 1 + (maxFT >= maxST ? maxFT : maxST);
}

int getNearestLeafLevel(TArb arb) {
    if (arb == NULL) return -1;  // if the quadtree is empty, return -1
    if (arb->type == 1)
        return 0;  // if the current node is leaf, the level is 0

    // compute the level of the closest leaf in each subtree
    int level1 = getNearestLeafLevel(arb->node1);
    int level2 = getNearestLeafLevel(arb->node2);
    int level3 = getNearestLeafLevel(arb->node3);
    int level4 = getNearestLeafLevel(arb->node4);

    /*  Computes the level of the closest leaf
     *  in the containing subtrees leaves
     */
    int nearestLeafLevel = -1;
    if (level1 != -1 && (nearestLeafLevel == -1 || level1 < nearestLeafLevel))
        nearestLeafLevel = level1;
    if (level2 != -1 && (nearestLeafLevel == -1 || level2 < nearestLeafLevel))
        nearestLeafLevel = level2;
    if (level3 != -1 && (nearestLeafLevel == -1 || level3 < nearestLeafLevel))
        nearestLeafLevel = level3;
    if (level4 != -1 && (nearestLeafLevel == -1 || level4 < nearestLeafLevel))
        nearestLeafLevel = level4;

    // if none of the subtrees have leaves, return -1
    if (nearestLeafLevel == -1) return -1;

    // returns the level of the closest leaf in the current node
    return nearestLeafLevel + 1;
}

void destroyTree(TArb arb) {
    // destroy the quadtree and free the memory
    if (!arb) return;
    destroyTree(arb->node1);
    destroyTree(arb->node2);
    destroyTree(arb->node3);
    destroyTree(arb->node4);
    free(arb);
}

// QUEUE FUNCTIONS

TQueue *initQ() {
    TQueue *queue;
    queue = (TQueue *)malloc(sizeof(TQueue));
    if (!queue) return NULL;

    /*  creates and allocates memory for the queue
     *  and sets its
     */

    queue->start = queue->end = NULL;
    return queue;
}

int intrQ(TQueue *queue, TArb arb) {
    //  inserts into queue pointer to quadtree cell parameter
    TList aux;
    aux = (TList)malloc(sizeof(TCell));
    if (!aux) return 0;
    aux->next = NULL;
    aux->arb = arb;

    if (queue->end != NULL)
        queue->end->next = aux;
    else
        queue->start = aux;
    queue->end = aux;
    return 1;
}

TArb extrQ(TQueue *queue, TArb arb) {
    /*  extracts from queue the first element and
     *  returns pointer to its quadtree cell
     */
    TList aux;
    aux = queue->start;
    arb = aux->arb;
    queue->start = queue->start->next;
    free(aux);
    return arb;
}

int verifQNull(TQueue *queue) {
    // verify if queue is null
    if (queue->start == NULL) return 1;
    return 0;
}

void resetQ(TQueue *queue) {
    // reset queue if empty
    TList list;
    list = queue->start;
    queue->start = NULL;
    queue->end = NULL;
    destroyList(&list);
}

void destroyList(TList *aL) {
    // destroy the linked list and free the memory
    TList list;
    while (*aL) {
        list = *aL;
        *aL = (*aL)->next;
        free(list->arb);
        free(list);
    }
}

void destrQ(TQueue **queue) {
    // destroy queue and free the memory
    TList list, aux;
    list = (*queue)->start;
    while (list) {
        aux = list;
        list = list->next;
        free(aux);
    }
    free(*queue);
    *queue = NULL;
}

// OTHER

void openFiles(FILE **in, FILE **out, const char *inFileName,
               const char *outFileName) {
    // opens the in and out binary files
    *in = fopen(inFileName, "rb");
    *out = fopen(outFileName, "wb");
    if (!in || !out) {
        fprintf(stderr, "ERROR: Unable to open binary file.");
        return;
    }
}

void solveC1(FILE *out, TArb arb, unsigned int height) {
    int i = 0, sizeOfBlock = 0;
    fprintf(out, "%d\n", numLevels(arb));  // displays number of levels
    fprintf(out, "%d\n", numLeafs(arb));   // displays number of leaves
    sizeOfBlock = height;
    for (i = 0; i < getNearestLeafLevel(arb); i++)
        sizeOfBlock = sizeOfBlock / 2;
    /*  displays the size of the square
     *  for the largest area in the image that
     *  remained undivided
     */
    fprintf(out, "%d\n", sizeOfBlock);
}