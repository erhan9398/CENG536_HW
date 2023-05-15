#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int matrix_length;
int number_of_thread;
int number_of_row_operation = 0;
int shift_number;
pthread_mutex_t row_mutex;
pthread_mutex_t column_mutex;
int** matrix;

void print_matrix() {
    for (int r = 0; r < matrix_length; r++) {
        for(int c = 0; c < matrix_length; c++)
            printf("%d ", matrix[r][c]);
        printf("\n");
    }
}

void* func(void* arg) {
    
    int thread_id = *(int*) arg;
    int number_of_shift_op = matrix_length / number_of_thread;
   
    // Enter the critical section
    pthread_mutex_lock(&row_mutex);
    
    int start = thread_id * number_of_shift_op;
    int end = start + number_of_shift_op;

    // calculate end row
    if (thread_id == number_of_thread - 1) {
        if (end < matrix_length) {
            end = matrix_length;
        }
    }

    // printf("thread id is %d\n", thread_id);
    // printf("start %d -- end %d\n", start, end);

    // Perform the row shift to the right
    for (int i = 0; i < shift_number; i++) {
        for (int row = start; row < end; row++) {
            // Store the last element of the row
            int temp = matrix[row][matrix_length - 1];

            // Shift the elements to the right
            for (int col = matrix_length - 1; col > 0; col--) {
                matrix[row][col] = matrix[row][col - 1];
            }

            // Move the last element to the first position
            matrix[row][0] = temp;
        }
    }

    // increase number of row operation
    number_of_row_operation++;

    // Exit the critical section
    pthread_mutex_unlock(&row_mutex);
       
    // Enter the critical section
    pthread_mutex_lock(&column_mutex);

    // wait for other threads to complete row operation
    while(number_of_row_operation < number_of_thread);

    // Shift the columns up
    for (int i = 0; i < shift_number; i++) {
        for (int col = start; col < end; col++) {
            // Store the first element of the column
            int temp = matrix[0][col];

            // Shift the elements down
            for (int row = 0; row < matrix_length - 1; row++) {
                matrix[row][col] = matrix[row + 1][col];
            }

            // Move the first element to the bottom of the column
            matrix[matrix_length - 1][col] = temp;
        }
    }
    // printf("###############thread id %d###################\n", thread_id);
    // print_matrix();
    
    // Exit the critical section
    pthread_mutex_unlock(&column_mutex);
    

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    
    // for execution times of threads
    clock_t start, end;
    double cpu_time_used;

    
    char line[256];
    FILE* file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return 1;
    }

    // Read the first line to get the number
    if (fgets(line, sizeof(line), file) != NULL)
        sscanf(line, "%d", &matrix_length);
    printf("matrix length is %d\n", matrix_length);
    
    // allocate memory dynamically for matrix
    matrix = (int**)malloc(matrix_length * sizeof(int*));
    for (int i = 0; i < matrix_length; i++)
        matrix[i] = (int*)malloc(matrix_length * sizeof(int));

    // Read the consecutive lines
    for (int i = 0; i < matrix_length; i++) {
        int number_of_column = 0;
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Unexpected end of file.\n");
            break;
        }
        char* token = strtok(line, " ");
        while(token != NULL && number_of_column < matrix_length) {
            sscanf(token, "%d", &matrix[i][number_of_column++]);
            token = strtok(NULL, " ");
        }
    }

    if (argc < 2) {
        printf("Please specify the number of threads.\n");
        printf("e.g. ./myapp <number of threads>\n");
        return -1;
    } else {
        sscanf(argv[1], "%d", &number_of_thread);
        sscanf(argv[2], "%d", &shift_number);
        printf("%d number of threads will be created\n", number_of_thread);
        printf("%d shift operation will be performed\n", shift_number);
    }

    // Initialize the mutex
    pthread_mutex_init(&row_mutex, NULL);
    pthread_mutex_init(&column_mutex, NULL);

    printf("before shift operation, matrix is \n");
    print_matrix();

    pthread_t threads[number_of_thread];
    int thread_id[number_of_thread];

    // start thread execution timer
    start = clock();
    for (int i = 0; i < number_of_thread; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, func, &thread_id[i]);
    }

    for (int i = 0; i < number_of_thread; i++) {
        pthread_join(threads[i], NULL);
    }
    // stop thread execution timer
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("after shift operation, matrix is \n");
    print_matrix();

    printf("number of threads: %d, Execution time: %f seconds\n", number_of_thread, cpu_time_used);
    
    // Destroy the mutex
    pthread_mutex_destroy(&row_mutex);
    pthread_mutex_destroy(&column_mutex);

    // deallocate dynamic memory
    for (int i = 0; i < matrix_length; i++)
        free(matrix[i]);
    free(matrix);

    fclose(file);


    return 0;
}
