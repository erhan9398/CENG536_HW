#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sorting.h"
#include "utils.h"

int** pipes; // global pipes to use also in signal handler
int shouldExit = 0;
int childIndex = 0;

void handle_signal(int signalNumber)
{
    // Get current time
    char timeString[20];
    char message[40]; 
    time_t currentTime = time(NULL);
    struct tm* timeinfo = localtime(&currentTime);
    strftime(timeString, sizeof(timeString), "%H:%M:%S", timeinfo);

    if (signalNumber == SIGUSR1) {
        snprintf(message, LEN(message), "SIGUSR1 %s", timeString);
        write(pipes[childIndex][1], message, LEN(message));
    }
    else if (signalNumber == SIGUSR2) {
        snprintf(message, LEN(message), "SIGUSR2 %s", timeString);
        write(pipes[childIndex][1], message, LEN(message));
    }
    else {
        printf("UNKNOWN SIGNAL!!\n");
    }
    shouldExit = 1;
}

int main(int argc, char* argv[])
{   
    int numberOfChild = 0;
    FILE *file;
    char fileName[20];

    // get number of child process from user
    if (argc > 1) {
        numberOfChild = strtol(argv[1], NULL, 10);
        printf("%d child process will be created\n", numberOfChild);
    }
    else {
        numberOfChild = 1;
        printf("please specify number of child process\n");
    }

    // generate n input files to use them as input for child processes
    generate_input_files(numberOfChild);

    int childPidArray[numberOfChild];
    pid_t pid;

    // allocate memory for pipes array
    pipes = (int**)malloc(sizeof(int*) * numberOfChild);
    for (int i = 0; i < numberOfChild; i++) {
        pipes[i] = (int*)malloc(2 * sizeof(int));
    }
    
    // open pipes
    for (int i = 0; i < numberOfChild; i++) {
        pipe(pipes[i]);
    }

    printf("child processes are sorting numbers and write informations to pipe...\n");

    // Fork child processes
    for (int i = 0; i < numberOfChild; i++)
    {
        // create child process
        pid = fork();

        // srand is required in order to generate random sleepTime for each child process
        srand(time(NULL)*(i+1));

        if (pid == -1)
        {
            perror("Failed to fork child process");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            childIndex = i;
            close(pipes[i][0]); // Close read end of pipe


            // open input<i>.txt file to read number to be sorted
            memset(fileName, '\0', LEN(fileName));
            sprintf(fileName,"file/input%d.txt", i);
            file = fopen(fileName, "r");
            if (file == NULL) {
                printf("Can not open input%d.txt file\n", i);
                return -1;
            }

            // generate random number in between 1-7 to sleep child processes
            int sleepTime = RANDOM_GENERATOR(7, 1);

            int numberOfInputs;
            // get the length of random unsorted numbers array from input file
            fscanf(file, "%d", &numberOfInputs);
            int inputNumberArray[numberOfInputs];


            // read input numbers from input<i>.txt file
            int inputIndex = 0;
            while(fscanf(file, "%d", &inputNumberArray[inputIndex]) == 1) {
                inputIndex++;
            }

            // get current child process pid
            pid = getpid();
            // register to signals
            signal(SIGUSR1, handle_signal);
            signal(SIGUSR2, handle_signal);

                
            // get time before sorting
            time_t startTime = time(NULL);
            // child processes with odd PID uses selection sort
            if (pid % 2) {
                selecetion_sort(inputNumberArray, numberOfInputs, sizeof(int), compare_by_value);
            }
            // child processes with even PID uses insertion sort
            else {
                insertion_sort(inputNumberArray, numberOfInputs, sizeof(int), compare_by_value);
            }
            // sleep random seconds after sorting
            sleep(sleepTime);
            time_t endTime = time(NULL);


            // calculate elaps time for sorting + sleep
            double executionTime = difftime(endTime, startTime);

            // write execution time to pipe 
            write(pipes[childIndex][1], &executionTime, sizeof(double));

            // write length of sorted array to pipe
            write(pipes[childIndex][1], &numberOfInputs, sizeof(int));
            
            // write sorted array of number to pipe
            write(pipes[childIndex][1], inputNumberArray, sizeof(inputNumberArray));

            while(!shouldExit) {}
            // close pipe
            close(pipes[childIndex][1]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            close(pipes[i][1]); // Close write end of pipe
            childPidArray[i] = pid;
        }
    }

    // create struct of execution time and child index to sort them according to execution time
    pipeOut_s pipesOutArray[numberOfChild];

    // read execution times from pipes
    for (int i = 0; i < numberOfChild; i++)
    {
        double exec;
        read(pipes[i][0], &exec, sizeof(double));
        pipesOutArray[i].index = i;
        pipesOutArray[i].executionTime = exec;
    }

    // read sorted array from pipes
    for (int i = 0; i < numberOfChild; i++)
    {
        // read length of sorted number array from pipre
        int lengthOfSortedArray = 0;
        read(pipes[i][0], &lengthOfSortedArray, sizeof(int));
        pipesOutArray[i].lengthOfSortedArray = lengthOfSortedArray;
        // allocate memory for sorted array 
        pipesOutArray[i].sortedNumArray = (int*)malloc(lengthOfSortedArray * sizeof(int));
        read(pipes[i][0], pipesOutArray[i].sortedNumArray, lengthOfSortedArray * sizeof(int));
    }
    
    // sort execution times
    insertion_sort(pipesOutArray, numberOfChild, sizeof(pipeOut_s), compare_for_pipe_out);

    printf("parent process waiting for child process to finish their jobs...\n");
    sleep(10);
    printf("parent process sends kill signal to child processes according to their PID even or odd...\n");

    // send kill signal to child process
    for (int i = 0; i < numberOfChild; i++) {
        if (childPidArray[i] % 2) // kill child process with odd pid by SIGUSR1
            kill(childPidArray[i], SIGUSR1);
        else    // kill child process even pid by SIGUSR2
            kill(childPidArray[i], SIGUSR2);
    }

    // read all pipes according to sorted index array, then write their context to 
    // single output.txt file
    FILE *finalFile = fopen("output.txt", "w");
    if (finalFile == NULL) {
        printf("can not create output.txt file\n");
        return -1;
    }

    printf("parent reads messages sent from child via pipes...\n");

    // read signal name and time from pipe
    for (int i = 0; i < numberOfChild; i++) {
        int currentSortedIndex = pipesOutArray[i].index;
        char signameAndTime[40];
        read(pipes[currentSortedIndex][0], signameAndTime, LEN(signameAndTime));
        // write execution time of current processed child
        fprintf(finalFile, "< %.2f > ", pipesOutArray[i].executionTime);
        
        fprintf(finalFile, "< ");
        // write sorted numbers to output.txt file
        for (int j = 0; j < pipesOutArray[i].lengthOfSortedArray; j++)
            fprintf(finalFile, "%d ", pipesOutArray[i].sortedNumArray[j]);
        fprintf(finalFile, "> ");

        // write the signal name and time come from parent
        // start index from 1, because first character will be new line in the buff
        fprintf(finalFile, "< ");
        fprintf(finalFile, "%s", signameAndTime);    
        fprintf(finalFile, " >");

        fprintf(finalFile, "\n");
    }

    printf("results are written to output.txt file by parent process\n");

    // we can close output.txt file
    fclose(finalFile);
    finalFile = NULL;

    // reap child processes
    int status;
    for(int i = 0; i < numberOfChild; i++) {
        waitpid(childPidArray[i], &status, 0);
    }

    // we need to deallocate dynamically allocated memories
    for (int i = 0; i < numberOfChild; i++) {
        free(pipes[i]);
    }
    free(pipes);
    for (int i = 0; i < numberOfChild; i++) {
        free(pipesOutArray[i].sortedNumArray);
    }

    return 0;
}
