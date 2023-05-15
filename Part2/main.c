#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

int** matrix1;
int** matrix2;
int** result_matrix;
int number_of_row;
int number_of_column;
int number_of_thread;
void print_matrix() {
    for (int r = 0; r < number_of_row; r++) {
        for(int c = 0; c < number_of_column; c++)
            printf("%d ", result_matrix[r][c]);
        printf("\n");
    }
}

// Thread function to add matrix rows
void* addRows(void* arg) {
    int row = *(int*)arg;

    // Add the corresponding rows of matrixA and matrixB
    for (int j = 0; j < number_of_column; j++) {
        result_matrix[row][j] = matrix1[row][j] + matrix2[row][j];
    }

    pthread_exit(NULL);
}

void* addAllRows(void* arg) {

    // Add the corresponding rows of matrixA and matrixB
    for (int i = 0; i < number_of_row; i++) {
        for (int j = 0; j < number_of_column; j++) {
            result_matrix[i][j] = matrix1[i][j] + matrix2[i][j];
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
    
    // Read the first line to get the number
    if (fgets(line, sizeof(line), file) != NULL)
        sscanf(line, "%d %d", &number_of_row, &number_of_column);
    printf("number_of_row is %d\nnumber_of_column is %d\n", number_of_row, number_of_column);

    // allocate memory dynamically for matrix
    matrix1 = (int**)malloc(number_of_row * sizeof(int*));
    for (int i = 0; i < number_of_row; i++)
        matrix1[i] = (int*)malloc(number_of_column * sizeof(int));
    matrix2 = (int**)malloc(number_of_row * sizeof(int*));
    for (int i = 0; i < number_of_row; i++)
        matrix2[i] = (int*)malloc(number_of_column * sizeof(int));
    result_matrix = (int**)malloc(number_of_row * sizeof(int*));
    for (int i = 0; i < number_of_row; i++)
        result_matrix[i] = (int*)malloc(number_of_column * sizeof(int));


    // Read the consecutive lines
    for (int i = 0; i < number_of_row; i++) {
        int column_cnt = 0;
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Unexpected end of file.\n");
            break;
        }

        char* token = strtok(line, " ");
        while(token != NULL && column_cnt < number_of_column) {
            int val;
            sscanf(token, "%d", &val);
            sscanf(token, "%d", &matrix1[i][column_cnt++]);
            token = strtok(NULL, " ");
        }
    }

    for (int i = 0; i < number_of_row; i++) {
        int column_cnt = 0;
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Unexpected end of file.\n");
            break;
        }
        char* token = strtok(line, " ");
        while(token != NULL && column_cnt < number_of_column) {
            sscanf(token, "%d", &matrix2[i][column_cnt++]);
            token = strtok(NULL, " ");
        }
    }

    pthread_t threads[number_of_row];
    int thread_id[number_of_row];
    // for execution times of threads
    clock_t start, end;
    double cpu_time_used;    

    // MULTI THREAD PART BEGINS

    // start thread execution timer
    start = clock();

    // Create threads to add matrix rows
    for (int i = 0; i < number_of_row; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, addRows, &thread_id[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < number_of_row; i++) {
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
    pthread_create(&single_thread, NULL, addAllRows, NULL);

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
    for (int i = 0; i < number_of_row; i++)
        free(matrix1[i]);
    for (int i = 0; i < number_of_row; i++)
        free(matrix2[i]);
    for (int i = 0; i < number_of_row; i++)
        free(result_matrix[i]);
    free(matrix1);
    free(matrix2);
    free(result_matrix);

    fclose(file);

    return 0;
}
