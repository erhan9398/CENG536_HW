#include <stdio.h>
#include <string.h>
#include "sorting.h"

void swap(void* a, void* b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

void selecetion_sort(void* arr, int length, size_t size, comparisonFunc compare) {

    for (int i = 0; i < length - 1; i++) {

        int minIndex = i;
        for (int j = i + 1; j < length; j++) {
            // Call the compare function to compare elements
            if(compare(arr + j * size, arr + minIndex * size) < 0) {
                minIndex = j;
            }
        }
        // Swap the elements
        if (minIndex != i) {
            swap(arr + i * size, arr + minIndex * size, size);
        }

    }
    
}

void insertion_sort(void* arr, int length, size_t size, comparisonFunc compare) {

    for (int i = 1; i < length; i++) {

        char key[size];
        memcpy(key, arr + i * size, size);
        int j = i - 1;

        while (j >= 0 && compare(arr + j * size, key) > 0) {
            memcpy(arr + (j + 1) * size, arr + j * size, size);
            j--;
        }
        memcpy(arr + (j + 1) * size, key, size);
    }
}

void print_array(int arr[], int length) {

    for (int i = 0; i < length; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}