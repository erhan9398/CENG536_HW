#pragma once
typedef int (*comparisonFunc)(const void*, const void*);
void selecetion_sort(void* arr, int length, size_t size, comparisonFunc compare);
void insertion_sort(void* arr, int length, size_t size, comparisonFunc compare);
void print_array(int arr[], int length);