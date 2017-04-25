#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if(argc < 3) {
        printf("Usage: slurm-check <input_file> <output_file_name>\n");
        exit(1);
    }

    float x, y;
    long z;

    long i;

    FILE* inputFile = fopen(argv[1], "r");
    while(fscanf(inputFile, "%ld,%lf,%lf", &i, &x, &y) == 3) {
    }
    fclose(inputFile);

    printf("Read %ld points from inputFile\n", (i+1));

    FILE* outputFile = fopen(argv[2], "w");
    fprintf(outputFile, "Read %ld points from inputFile\n", (i+1));
    fclose(outputFile);

    return 0;
}
