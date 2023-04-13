#pragma once
#include "random_generator.h"

#define LEN(x) (sizeof(x)/sizeof(x[0]))

// define struct to keep index array index and execution times to compare and sort them accordingly
typedef struct indexAndExecTime {
    int index;
    double executionTime;
}indexAndExecTime_s;

// define struct to keep index, execution times and array of sorted number
typedef struct pipesOut 
{
    int index;
    double executionTime;
    int* sortedNumArray;
    int lengthOfSortedArray;
}pipeOut_s;


void generate_input_files(int numberOfInputs);
int compare_by_value(const void* in1, const void* in2);
int compare_by_execution(const void* in1, const void* in2);
int compare_for_pipe_out(const void* in1, const void* in2);