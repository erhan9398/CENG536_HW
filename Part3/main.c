#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

int** matrix1;
int** matrix2;
int** result_matrix;
int number_of_row1;
int number_of_column1;
int number_of_row2;
int number_of_column2;
int number_of_thread;
pthread_mutex_t mutex;
void print_matrix() {
    for (int r = 0; r < number_of_row1; r++) {
        for(int c = 0; c < number_of_column2; c++)
            printf("%d ", result_matrix[r][c]);
        printf("\n");
    }
}

// Thread function to add matrix rows
void* multiplyRows(void* arg) {
    int row = *(int*)arg;

    // multiply the corresponding rows and column of matrixA and matrixB
    int temp = 0;
    for (int j = 0; j < number_of_column2; j++) {
        
        for (int i = 0; i < number_of_column1; i++) {
            temp += (matrix1[row][i] * matrix2[i][j]);
        }
        // Enter the critical section
        pthread_mutex_lock(&mutex);
        result_matrix[row][j] = temp;
        temp = 0;
        // Exit the critical section
        pthread_mutex_unlock(&mutex);
    }


    pthread_exit(NULL);
}

// Thread function to add matrix rows
void* multiplyAllRows(void* arg) {

    // multiply the corresponding rows and column of matrixA and matrixB
    int temp = 0;
    for (int row = 0; row < number_of_row1; row++) {
        for (int j = 0; j < number_of_column2; j++) {
            for (int i = 0; i < number_of_column1; i++) {
                temp += (matrix1[row][i] * matrix2[i][j]);
            }

            result_matrix[row][j] = temp;
            temp = 0;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    char line[256];
    FILE* file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return 1;
    }
    
    // Read the first line to get the number of rows and column of matrix1
    if (fgets(line, sizeof(line), file) != NULL)
        sscanf(line, "%d %d", &number_of_row1, &number_of_column1);
    printf("number_of_row1 is %d\nnumber_of_column1 is %d\n", number_of_row1, number_of_column1);

    // allocate memory dynamically for matrix1
    matrix1 = (int**)malloc(number_of_row1 * sizeof(int*));
    for (int i = 0; i < number_of_row1; i++)
        matrix1[i] = (int*)malloc(number_of_column1 * sizeof(int));

    // Read the consecutive lines for matrix1
    for (int i = 0; i < number_of_row1; i++) {
        int column_cnt = 0;
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Unexpected end of file.\n");
            break;
        }

        char* token = strtok(line, " ");
        while(token != NULL && column_cnt < number_of_column1) {
            int val;
            sscanf(token, "%d", &val);
            sscanf(token, "%d", &matrix1[i][column_cnt++]);
            token = strtok(NULL, " ");
        }
    }
    
    // Read the first line to get the number of rows and column of matrix2
    if (fgets(line, sizeof(line), file) != NULL)
        sscanf(line, "%d %d", &number_of_row2, &number_of_column2);
    printf("number_of_row2 is %d\nnumber_of_column2 is %d\n", number_of_row2, number_of_column2);

    if (number_of_column1 != number_of_row2) {
        printf("ERROR!!! matrix's formats must be m by n & n by k\n");
        return -1;
    }

    // allocate memory dynamically for matrix2
    matrix2 = (int**)malloc(number_of_row2 * sizeof(int*));
    for (int i = 0; i < number_of_row2; i++)
        matrix2[i] = (int*)malloc(number_of_column2 * sizeof(int));

    // Read the consecutive lines for matrix 2
    for (int i = 0; i < number_of_row2; i++) {
        int column_cnt = 0;
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Unexpected end of file.\n");
            break;
        }

        char* token = strtok(line, " ");
        while(token != NULL && column_cnt < number_of_column2) {
            int val;
            sscanf(token, "%d", &val);
            sscanf(token, "%d", &matrix2[i][column_cnt++]);
            token = strtok(NULL, " ");
        }
    }
    
    // allocate memory dynamically for result matrix
    result_matrix = (int**)malloc(number_of_row1 * sizeof(int*));
    for (int i = 0; i < number_of_row1; i++)
        result_matrix[i] = (int*)malloc(number_of_column2 * sizeof(int));

    pthread_t threads[number_of_row1];
    int thread_id[number_of_row1];
    // for execution times of threads
    clock_t start, end;
    double cpu_time_used;    
    
    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);
    
    // MULTI THREAD PART BEGINS

    // start thread execution timer
    start = clock();

    // Create threads to add matrix rows
    for (int i = 0; i < number_of_row1; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, multiplyRows, &thread_id[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < number_of_row1; i++) {
        pthread_join(threads[i], NULL);
    }

    // stop thread execution timer
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    
    // Print the resulting matrix
    printf("Result Matrix:\n");
    print_matrix();
    // Print the execution time
    printf("Multi thread Execution Time: %.6f sec.\n", cpu_time_used);

    // MULTI THREAD PART ENDS

    // SINGLE THREAD PART BEGINS

    // start thread execution timer
    start = clock();

    // Create threads to add matrix rows
    pthread_t single_thread;
    pthread_create(&single_thread, NULL, multiplyAllRows, NULL);

    // Wait for thread to complete
    pthread_join(single_thread, NULL);

    // stop thread execution timer
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    
    // Print the resulting matrix
    printf("Result Matrix:\n");
    print_matrix();
    // Print the execution time
    printf("Single thread Execution Time: %.6f sec.\n", cpu_time_used);

    // SINGLE THREAD PART ENDS

    // deallocate dynamic memory
    for (int i = 0; i < number_of_row1; i++)
        free(matrix1[i]);
    for (int i = 0; i < number_of_row2; i++)
        free(matrix2[i]);
    for (int i = 0; i < number_of_row1; i++)
        free(result_matrix[i]);
    free(matrix1);
    free(matrix2);
    free(result_matrix);

    fclose(file);

    return 0;
}
