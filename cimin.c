#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void InputAnalysis(int argc, char* argv[], char* returnArr[4]) {
    bool receivedParam = true;

    if (argc < 7) {
        printf("Usage: %s -i inputPath -m errorString -o outputPath programToRun\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc-1; i++) {
        if (argv[i][0] == '-') {
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
            receivedParam = true;
        }
        else {
            // Check if the current argument is an argument following a parameter
            if (!receivedParam) {
                printf("Invalid argument: %s\n", argv[i]);
                exit(1);
            }
            receivedParam = false;
        }
    }
    if (strcmp(argv[argc-1], "a.out")==0) {
        returnArr[3] = argv[argc-1];
    }
    if (returnArr[3]==NULL){
    	printf("Missing argument for %s parameter\n", argv[5]);
    	exit(1);
    }
}

int main(int argc, char* argv[]) {
    char* inputs[4] = { NULL, NULL, NULL, NULL };
    printf("argc:%d\n",argc);
    InputAnalysis(argc, argv, inputs);

    printf("Input path: %s\nError String: %s\nOutput path: %s\nProgram to be executed: %s\n", inputs[0], inputs[1], inputs[2], inputs[3]);

    return 0;
}
