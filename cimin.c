#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <error.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

//two pipes needed => pipe[0]:standard input, pipe[1]:standard output
int pipes[2];

bool ExecutePrgm(char* t, char* prgm){
    pid_t child=fork();
    if(child<0) {
        perror("Error(fork)");
        exit(1);
    } else if (child == 0){
        //child process
        close(pipes[0]);	//close reading pipe
        dup2(pipes[1],1);	//redirect stdout to writing pipe
        close(pipes[1]);	//close writing pipe
        execvp(prgm,t); //run program
        exit(33);
    } else{
        //parent process
        int status;
        wait(&status);
        if(WEXITSTATUS(status) == 33){
            //found the error string (crashes)
            return true;
        }
        else return false;
    }
}

char* Reduce(char* t, char* prgm){
    int s = strlen(t) - 1;
    char* head;
    char* mid;
    char* tail;

    while(s > 0){
        for(int i = 0; i < strlen(t) - s - 1;i++){
            //allocate memory for each part
            head = (char*)malloc(sizeof(char)*i+1);
            tail = (char*)malloc(sizeof(char)*(strlen(t)-i-s)+1);
            
            //copy the substrings to each part
            strncpy(head,t,i);
            strncpy(tail,t+i+s,strlen(t)-i-s);

            //foolproof method to make sure the strings are null terminated
            head[i] = '\0';
            tail[strlen(t)-i-s] = '\0';

            strcat(head,tail);	//concatenate head and tail
            
            //if the string with the problem is found, reduce it further
            //(crashes)
            if(ExecutePrgm(head,prgm)) {
                printf("\nfound error string: %s...\nNow reducing further",head);
                return Reduce(head,prgm);	
            }

        }
        for(int i = 0; i < strlen(t) - s - 1 ;i++){
            //find middle part
            mid = (char*)malloc(sizeof(char)*s+1);
            strncpy(mid,t+i,s);
            mid[s] = '\0';

            //try to find the error string in the middle part
            if(ExecutePrgm(mid,prgm)) 
            {
                printf("\nfound error string: %s...\nNow reducing further",mid);
                return Reduce(mid,prgm);
            }
        }
        s--;
    }
    printf("\nFinished reducing. Error string is: %s\n")
    return t;
}

void InputAnalysis(int argc, int paramlen, char* argv[], char* returnArr[4], char* tarArg[paramlen]) {
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
		for(int k=0; k<paramlen; k++){
		    tarArg[k]=argv[i+k+1]; 	//storing parameters for target program
		}
		i+=3;
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

void timeout(int sig) {	//when SIGALRM(timeout) happens
    if(sig==SIGALRM) puts("Time out!");
    exit(0);
}
void terminate(int sig) { //when SIGINT(control+c) happens
    if(sig==SIGINT) printf("\nCTRL+C is pressed!\nexit process..\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    // pid_t child_pid;

    signal(SIGINT,terminate);	//when SIGINT, calls terminate to exit
    signal(SIGALRM,timeout);	//when SIGALRM, calls timeout to exit
    alarm(3);

    if(pipe(pipes)!=0){
    	perror("Error(pipe generation)");
	exit(1);
    }
    printf("reading pipe: %d, writing pipe: %d\n",pipes[0],pipes[1]);

    char* inputs[4] = { NULL, NULL, NULL, NULL };
    int paramlen=argc-8;
    char* tarArg[paramlen];

    printf("argc: %d, parameter length: %d\n",argc,paramlen);
    InputAnalysis(argc, paramlen, argv, inputs, tarArg);

    printf("parameters of target program: ");
    for(int i=0;i<paramlen;i++){
    	printf("%s, ",tarArg[i]);
    }
    printf("\n");

    printf("Input path: %s\nError String: %s\nOutput path: %s\nProgram to be executed: %s\n", inputs[0], inputs[1], inputs[2], inputs[3]);
     
     /* Reading Input File */
    char crashInput[4097];	//array for reading crashing input
    int crlen;			//length of crahsing input
    FILE* crash=fopen(inputs[0],"r");
    if(crash==NULL){
        perror("No file; exiting..\n");
        exit(1);
    }
    crlen=fread(crashInput,sizeof(char),4096,crash);
    
    if(crlen!=0){	//fread successful
    	printf("length of crashing input: %d\n", crlen);
        printf("content: %s\n", crashInput);
        fclose(crash);
    }
    else{		//fread failed
        if(ferror(crash)) perror("Error reading crashing input file\n");
        else if(feof(crash)) perror("EOF found\n");
    }

    crashInput[crlen]='\0';
    printf("crashing input: %s\n",crashInput);

    printf("check if input is printed\n");

    ssize_t s;
    char buf[1024];

    char* resultStr = Reduce(crashInput,inputs[3]);
    printf("\n\n******Result******\n%s\n\n******************",resultStr);
    // child_pid=fork();
    // if(child_pid<0){
    // 	perror("Fork Error");
    //     exit(1);
    // }
    // if(child_pid==0){  			//child process
	
    //     close(pipes[0]);                //close reading pipe
    // 	dup2(pipes[1],STDERR_FILENO); 	//standard error -> writing pipe

    //     execvp(inputs[3],tarArg);	//executing jsondump in child process
    //     perror("execl failed");
    //     exit(1);
    // }
    // else{				//parent process
    // 	wait(0x0);			//wait until the child process is done
    // 	close(pipes[1]); 		//close writing pipe
	
    // 	while(1){
    // 	    s=read(pipes[0],buf,1023);
	//     buf[s+1]=0x0;
    // 	    printf("stderr> %s\n",buf);
	//     sleep(5);
	// //write(pipes[1],input,4096);
	// }
    // }

    return 0;

}
