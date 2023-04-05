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

//two pipes are needed => one for stderr, one for passing crashing input to the target program
int pipes[2];
int crin[2];
//pipe[0]:standard input, pipe[1]:standard output

bool ExecutePrgm(char* crash, char* prgm, char* param){
    ssize_t s;
    char buf[1024];

    pid_t child=fork();
    if(child<0) {
        perror("Error(fork)");
        exit(1);
    }
    else if (child == 0){	 //child process
	/*
        close(pipes[0]);	//close reading pipe
        dup2(pipes[1],1);	//redirect stdout to writing pipe
        close(pipes[1]);	//close writing pipe
        execvp(prgm,t); //run program
        exit(33);
	*/
	close(crin[1]);			//close writing pipe so it can read crashing input form pipe
	dup2(crin[0],STDIN_FILENO);	//standard input -> reading pipe
	
	execv(/*inputs[3], tarArg*/prgm,param);	//executing jsondump in child process
	perror("execvp failed");
	exit(/*1*/33);
    }
    else{		//parent process
	close(pipes[1]); 		//close writing pipe
	close(crin[0]);			//close reading pipe to write crashing input to pipe

	int check=write(crin[1]/*,crashInput,crlen+1*/,crash,strlen(crash)+1);	//write crashing input to writing pipe of crin
	if(check!=strlen(crash)+1/*crlen+1*/) perror("write malfunctioning");
	close(crin[1]);				//since writing to crin is done, close writing pipe
	
	int sum=0;
    	while(sum<check){
    	    s=read(pipes[0],buf,1023);
	    sum+=s;
	    buf[s+1]=0x0;
    	    printf("stderr> %s",buf);
	}	

        int status;
        wait(&status);
        if(WEXITSTATUS(status) == 33){
            //found the error string (crashes)
            return true;
        }
        else return false;
    }
}

char* Reduce(char* crash, char* prgm, char* param){
    int s = strlen(crash) - 1;
    char* head;
    char* mid;
    char* tail;

    while(s > 0){
        for(int i = 0; i < strlen(crash) - s - 1;i++){
            //allocate memory for each part
            head = (char*)malloc(sizeof(char)*i+1);
            tail = (char*)malloc(sizeof(char)*(strlen(crash)-i-s)+1);
            
            //copy the substrings to each part
            strncpy(head,crash,i);
            strncpy(tail,crash+i+s,strlen(crash)-i-s);

            //foolproof method to make sure the strings are null terminated
            head[i] = '\0';
            tail[strlen(crash)-i-s] = '\0';

            strcat(head,tail);	//concatenate head and tail
            
            //if the string with the problem is found, reduce it further
            //(crashes)
            if(ExecutePrgm(head,prgm,param)) {
                printf("\nfound error string: %s...\nNow reducing further",head);
                return Reduce(head,prgm,param);	
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
                return Reduce(mid,prgm,param);
            }
        }
        s--;
    }
    printf("\nFinished reducing. Error string is: %s\n",mid);
    return mid;
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
		tarArg[paramlen]=NULL;
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
    pid_t child_pid;

    signal(SIGINT,terminate);	//when SIGINT, calls terminate to exit
    signal(SIGALRM,timeout);	//when SIGALRM, calls timeout to exit
    alarm(3);

    if(pipe(pipes)!=0){
    	perror("Error(pipe generation)");
	exit(1);
    }
    printf("1st pipe(for stderr) => reading pipe: %d, writing pipe: %d\n",pipes[0],pipes[1]);

    if(pipe(crin)!=0){
	perror("Error(pipe generation)");
	exit(1);
    }
    printf("2nd pipe(for crashing input) => reading pipe: %d, writing pipe: %d\n",crin[0],crin[1]);

    char* inputs[4] = { NULL, NULL, NULL, NULL };
    int paramlen=argc-8;
    char* tarArg[paramlen+1];

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

    printf("check if input is printed\n");

    //implemented in Execute_Prgm
    /*
    ssize_t s;
    char buf[1024];

    child_pid=fork();
    if(child_pid<0){
    	perror("Fork Error");
        exit(1);
    }
    if(child_pid==0){  			//child process
	close(crin[1]);			//close writing pipe so it can read crashing input form pipe
	dup2(crin[0],STDIN_FILENO);	//standard input -> reading pipe
	
	execv(inputs[3], tarArg);	//executing jsondump in child process
	perror("execvp failed");
	exit(1);
    }
    else{				//parent process
    	close(pipes[1]); 		//close writing pipe
	close(crin[0]);			//close reading pipe to write crashing input to pipe

	int check=write(crin[1],crashInput,crlen+1);	//write crashing input to writing pipe of crin
	if(check!=crlen+1) perror("write malfunctioning");
	close(crin[1]);				//since writing to crin is done, close writing pipe
	
	int sum=0;
    	while(sum<check){
    	    s=read(pipes[0],buf,1023);
	    sum+=s;
	    buf[s+1]=0x0;
    	    printf("stderr> %s",buf);
	}

	wait(0x0);			//wait until the child process is done
    }
    */
    Reduce(crashInput,inputs[3],tarArg);
   
    return 0;

}
