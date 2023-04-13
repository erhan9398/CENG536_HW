#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sorting.h"
#include "utils.h"

// to make child process exit from infinite loop
int shouldExit = 0;
FILE* outputFile;

void signal_handler(int signalNumber) {
    
    // Get current time
    char timeString[20]; 
    time_t currentTime = time(NULL);
    struct tm* timeinfo = localtime(&currentTime);
    strftime(timeString, sizeof(timeString), "%H:%M:%S", timeinfo);

    if (signalNumber == SIGUSR1) {
        fprintf(outputFile, "SIGUSR1 %s", timeString);
    }
    else if (signalNumber == SIGUSR2) {
        fprintf(outputFile, "SIGUSR2 %s", timeString);
    }
    else {
        fprintf(outputFile, "UNKNOWN SIGNAL!!");
    }
    shouldExit = 1;
}

int main(int argc, char* argv[]) {
    
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

    printf("child processes are sorting numbers and write informations to file...\n");

    for (int i = 0; i < numberOfChild; i++) {
        
        // create child process
        pid = fork();

        // srand is required in order to generate random sleepTime for each child process
        srand(time(NULL)*(i+1));

        // child process
        if (pid == 0) {

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

            // we can close input file
            fclose(file);
            file = NULL;

            // get current child process pid
            pid = getpid();
            
            // register both SIGUSR1 & SIGUSR2 signals
            signal(SIGUSR1, signal_handler);
            signal(SIGUSR2, signal_handler);
                
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
            
            // open output<i>.txt file to write sorted number and execution time
            memset(fileName, '\0', LEN(fileName));
            sprintf(fileName,"file/output%d.txt", i);
            outputFile = fopen(fileName, "w");
            if (outputFile == NULL) {
                printf("Can not open output%d.txt file\n", i);
                return -1;
            }

            // write sorted numbers to output<i>.txt file
            fprintf(outputFile, "%d\n", numberOfInputs);
            for (int j = 0; j < numberOfInputs; j++)
                fprintf(outputFile, "%d ", inputNumberArray[j]);
            fprintf(outputFile, "\n");
            // write execution time to output<i>.txt file
            fprintf(outputFile, "%.2f\n", executionTime);
            
            // wait until kill signal come from parent process
            while(!shouldExit){}
            fclose(outputFile);
            outputFile = NULL;
            exit(0);
        } 
        else {
            // parent process
            // save child process pids to use later
            childPidArray[i] = pid;
        }
    }

    // wait for child process to finish their jobs
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

    // reap child processes
    int status;
    for(int i = 0; i < numberOfChild; i++) {
        waitpid(childPidArray[i], &status, 0);
    }

    printf("parent reads messages sent from child via files...\n");

    // after all child process reaped then we can read all ouput<i>.txt files
    // and generate ouput.txt file

    // create struct of execution time and child index to sort them according to execution time
    indexAndExecTime_s execTimeAndIndexArray[numberOfChild];

    for (int i = 0; i < numberOfChild; i++) {
        // save index of child process
        execTimeAndIndexArray[i].index = i;
        
        memset(fileName, '\0', LEN(fileName));
        sprintf(fileName,"file/output%d.txt", i);
        file = fopen(fileName, "r");
        if (file == NULL) {
            printf("Can not open output%d.txt file\n", i);
            return -1;
        }

        // Read lines from the file until execution time line is reached
        int lineNumber = 3; // the line number 3 (for execution time)
        int currentLine = 1; // current line counter
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        // Read lines from the file using getline
        while ((getline(&line, &len, file)) != -1) {
            if (++currentLine == lineNumber) {
                fscanf(file, "%le", &execTimeAndIndexArray[i].executionTime);
                break;
            }                   
        }
        fclose(file);
    }

    // sort execution times
    insertion_sort(execTimeAndIndexArray, numberOfChild, sizeof(indexAndExecTime_s), compare_by_execution);

    // read all output<i>.txt files according to sorted index array, then write their context to 
    // single output.txt file
    FILE *finalFile = fopen("output.txt", "w");
    if (finalFile == NULL) {
        printf("can not create output.txt file\n");
        return -1;
    }

    for (int i = 0; i < numberOfChild; i ++) {
        // read intermediate file according to execution time and write them to output.txt
        memset(fileName, '\0', LEN(fileName));
        sprintf(fileName, "file/output%d.txt", execTimeAndIndexArray[i].index);
        file = fopen(fileName, "r");
        if (file == NULL) {
            printf("can not open file/output%d.txt file\n", execTimeAndIndexArray[i].index);
        }

        // get the length of sorted numbers array from output<i>.txt file
        int numberOfSortedOutput;
        fscanf(file, "%d", &numberOfSortedOutput);
        int outputNumberArray[numberOfSortedOutput];
        
        // read sorted numbers from output<i>.txt file
        int numberIndex = 0;
        while(fscanf(file, "%d", &outputNumberArray[numberIndex]) == 1) {
            numberIndex++;
        }

        // read execution time form output<i>.txt file
        double execTime;
        fscanf(file, "%le", &execTime);

        // read signal name and receive time from output<i>.txt file
        char buff[100]; 
        numberIndex = 0;
        while(fscanf(file, "%c", &buff[numberIndex]) == 1) {
            numberIndex++;
        }

        // we can close output<i>.txt file
        fclose(file);
        file = NULL;

        // write execution time of current processed child
        fprintf(finalFile, "< %.2f > ", execTimeAndIndexArray[i].executionTime);
        
        fprintf(finalFile, "< ");
        // write sorted numbers to output.txt file
        for (int j = 0; j < numberOfSortedOutput; j++)
            fprintf(finalFile, "%d ", outputNumberArray[j]);
        fprintf(finalFile, "> ");

        // write the signal name and time come from parent
        // start index from 1, because first character will be new line in the buff
        fprintf(finalFile, "< ");
        for (int j = 1; j < numberIndex; j++)
            fprintf(finalFile, "%c", buff[j]);    
        fprintf(finalFile, " >");

        fprintf(finalFile, "\n");
    }

    // we do all work with intermediate output<i>.txt file, we can delete them
    for (int i = 0; i < numberOfChild; i++) {
        memset(fileName, '\0', LEN(fileName));
        sprintf(fileName, "file/output%d.txt", i);
        remove(fileName);
    }

    // we can close output.txt file
    fclose(finalFile);

    printf("results are written to output.txt file by parent process\n");

    return 0;
}