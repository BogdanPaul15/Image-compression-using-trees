#include "quadtree.h"

int main(int argc, char const *argv[]) {
    FILE *in, *out;
    int factor = 0;
    unsigned int width = 0, height = 0, i = 0;
    if (strcmp(argv[1], "-c1") == 0) {
        // open specific files for in and out
        openFiles(&in, &out, argv[3], argv[4]);

        // create and allocate the pixel matrix
        RGB **pic = prepareMatrix(in, out, &height, &width, argv[2], &factor);

        // create the quadtree based on the pixel matrix
        TArb arb = constrNodeInt();
        arb = divideQuadtree(pic, arb, 0, 0, height, height, height, factor);

        /*  display the number of levels, leaves and dimension
         *  of the max square that remained undivised
         */
        solveC1(out, arb, height);

        // free memory
        destroyTree(arb);             // free quadtree
        for (i = 0; i < height; i++)  // free pixel matrix
            free(pic[i]);
        free(pic);
    } else if (strcmp(argv[1], "-c2") == 0) {
        // open specific files for in and out
        openFiles(&in, &out, argv[3], argv[4]);

        // create and allocate the pixel matrix
        RGB **pic = prepareMatrix(in, out, &height, &width, argv[2], &factor);

        // create the quadtree based on the pixel matrix
        TArb arb = constrNodeInt();
        arb = divideQuadtree(pic, arb, 0, 0, height, height, height, factor);

        // write image dimension and breadth first traversal of the quadtree
        fwrite(&height, sizeof(unsigned int), 1, out);
        breadthFirst(out, arb);

        // free memory
        destroyTree(arb);             // free quadtree
        for (i = 0; i < height; i++)  // free pixel matrix
            free(pic[i]);
        free(pic);
    } else if (strcmp(argv[1], "-d") == 0) {
        unsigned int size = 0;
        // open specific files for in and out
        openFiles(&in, &out, argv[2], argv[3]);

        // create quadtree based on the breadth first traversal
        TArb arb = createQuadtree(in, &size);

        // create image based on the quadtree
        createImage(out, arb, size);

        // free memory
        destroyTree(arb);  // free quadtree
    } else {
        fprintf(stderr, "Please use a standard option.");
    }
    fclose(in);
    fclose(out);
    return 0;
}