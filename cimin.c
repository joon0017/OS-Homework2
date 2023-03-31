#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <error.h>

//three pipes needed => pipe[0]:standard input, pipe[1]:standard output, pipe[2]:standard error
int pipes[3];

char* Reduce(char* t){
    int s = strlen(t) - 1;

    while (s > 0){
        char* temp = (char*)malloc(sizeof(char) * s);
        
    }


}

void InputAnalysis(int argc, char* argv[], char* returnArr[4]) {
    bool receivedParam = false;
    bool receivedProgram = false;

    if (argc < 7) {
        printf("Usage: %s -i inputPath -m errorString -o outputPath programToRun\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            receivedParam = true;
            if (i + 1 >= argc) {
                printf("Missing argument for %s parameter.\n", argv[i]);
                exit(1);
            }

            // Check the parameter and assign the argument to the correct variable
            if (strcmp(argv[i], "-i") == 0) {
                returnArr[0] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-m") == 0) {
                returnArr[1] = argv[i + 1];
            }
            else if (strcmp(argv[i], "-o") == 0) {
                returnArr[2] = argv[i + 1];
            }
            else {
                printf("Invalid parameter: %s\n", argv[i]);
                exit(1);
            }
            i++;
        }
        else {
            // Check if the current argument is an argument following a parameter
            if (!receivedProgram) {
                returnArr[3] = argv[i];
                receivedProgram = true;
            }
            receivedParam = false;
        }
    }
    if (returnArr[3]==NULL){
    	printf("Missing argument for executable program parameter\n");
    	exit(1);
    }
}

int main(int argc, char* argv[]) {
    pid_t child_pid;

    if(pipe(pipes)!=0){
    	perror("Error(pipe generation)");
	exit(1);
    }
    printf("reading pipe: %d, writing pipe: %d, error pipe: %d\n",pipes[0],pipes[1],pipes[2]);

    char* inputs[4] = { NULL, NULL, NULL, NULL };
    printf("argc: %d\n",argc);
    InputAnalysis(argc, argv, inputs);

    printf("Input path: %s\nError String: %s\nOutput path: %s\nProgram to be executed: %s\n", inputs[0], inputs[1], inputs[2], inputs[3]);

    child_pid=fork();
    if(child_pid<0){
    	perror("Fork Error");
	exit(1);
    }
    if(child_pid==0){  //child process
    	
    }
    wait(0x0);	  //wait until the child process is done
    return 0;
}
