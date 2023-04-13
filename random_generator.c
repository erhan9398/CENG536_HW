#include "random_generator.h"

void generate_random_numbers(random_generator specs, int index) {

    srand(time(NULL)*(index + 1));

    int arr[100];
    int size = RANDOM_GENERATOR(specs.sizeMax, specs.sizeMin);
    
    FILE* inputFile;
    char fileName[50];
    
    sprintf(fileName, "file/input%d.txt", index);
    inputFile = fopen(fileName, "w");
    if (inputFile == NULL) {
        printf("error opening file %s\n", fileName);
        return;
    }
    
    fprintf(inputFile, "%d\n", size);
    for (int i = 0; i < size; i++) {
        arr[i] = RANDOM_GENERATOR(specs.rangeMax, specs.rangeMin);
        fprintf(inputFile, "%d ", arr[i]);
    }
    fclose(inputFile);
}