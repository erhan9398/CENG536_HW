#include "utils.h"

void generate_input_files(int numberOfInputs) {
    
    // this part is used for generating random numbers
    // and write them into input<i>.txt files
    random_generator specs = {
        .rangeMin = 0,
        .rangeMax = 100,
        .sizeMin = 8,
        .sizeMax = 20
    };
    
    for (int i = 0; i < numberOfInputs; i++) {
        generate_random_numbers(specs, i);
    }
}

// this comparison function is for integer compare to use sorting algorithms
int compare_by_value(const void* in1, const void* in2) {
    int retVal = 0;
    const int val1 = *(const int*)in1;
    const int val2 = *(const int*)in2;
    if (val1 > val2) retVal = 1;
    if (val1 < val2) retVal = -1;
    return retVal;
}

int compare_by_execution(const void* in1, const void* in2) {
    int retVal = 0;
    const indexAndExecTime_s element1 = *(const indexAndExecTime_s*)in1;
    const indexAndExecTime_s element2 = *(const indexAndExecTime_s*)in2;
    if (element1.executionTime > element2.executionTime) retVal = 1;
    if (element1.executionTime < element2.executionTime) retVal = -1;
    return retVal;
}

int compare_for_pipe_out(const void* in1, const void* in2) {
    int retVal = 0;
    const pipeOut_s element1 = *(const pipeOut_s*)in1;
    const pipeOut_s element2 = *(const pipeOut_s*)in2;
    if (element1.executionTime > element2.executionTime) retVal = 1;
    if (element1.executionTime < element2.executionTime) retVal = -1;
    return retVal;
}