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

#ifdef DEBUG
#define DPRINT(func) func;
#else
#define DPRINT(func);
#endif

bool checkloop;                 //global variable for checking infinite loop
bool ctrlc=false;               //global variable for checking if ctrl+c is pressed
bool running;                   //global variable for checking if child is finished or not
pid_t child;                    //global variable for fork() and timeout()
int count=0;                      //global variable for counting total crashes

//two pipes are needed => one for stderr, one for passing crashing input to the target program
//int pipes[2];
//int crin[2]; <= the crashing input pipe is declared in each ExecutePrgm call
//pipe[0]:standard input, pipe[1]:standard output

void timeout(int sig);
void terminate(int sig);

bool ExecutePrgm(char* crash, char* prgm, char** param, char* errmsg){
    printf(" %s \n", crash);
    checkloop=false;

    int pipes[2];
    int crin[2]; 
    
    ssize_t s; 
    char out[4096];
    char buf[100];  

    signal(SIGINT,terminate);   //when SIGINT, calls terminate handling function
    signal(SIGALRM,timeout);    //when SIGALRM, calls timeout handling function
    alarm(3);

    if(pipe(pipes)!=0){
        perror("Error(pipe generation)");
        exit(1);
    }
    DPRINT( printf("1st pipe(for stderr) => reading pipe: %d, writing pipe: %d\n",pipes[0],pipes[1]); );

    if(pipe(crin)!=0){
        perror("Error(pipe generation)");
        exit(1);
    }
    DPRINT(printf("2nd pipe(for crashing input) => reading pipe: %d, writing pipe: %d\n",crin[0],crin[1]); );

    child=fork();
    running=true;              //child is running

    if(child<0) {
        perror("Error(fork)");
        exit(1);
    }
    else if (child == 0){	    //child process
        /*
            close(pipes[0]);	//close reading pipe
            dup2(pipes[1],1);	//redirect stdout to writing pipe
            close(pipes[1]);	//close writing pipe
            execvp(prgm,t);     //run program
            exit(33);
        */
        close(crin[1]);			    //close writing pipe so it can read crashing input form pipe
        dup2(crin[0],STDIN_FILENO);	//standard input -> reading pipe
        
        close(pipes[0]);                //close reading pipe
        dup2(pipes[1],STDERR_FILENO);   //standard error -> writing pipe
        close(STDOUT_FILENO);
        execv(/*inputs[3], tarArg*/prgm,param);	//executing jsondump in child process
        perror("execvp failed");
        exit(/*1*/33);
    }
    else{		//parent process
        close(pipes[1]); 		//close writing pipe
        close(crin[0]);			//close reading pipe to write crashing input to pipe

        int check=write(crin[1]/*,crashInput,crlen+1*/,crash,strlen(crash)+1);	//write crashing input to writing pipe of crin
        DPRINT( printf("written length of crash input: %d\n",check); );
        if(check!=strlen(crash)+1/*crlen+1*/) perror("write malfunctioning");
        close(crin[1]);				//since writing to crin is done, close writing pipe
        
        DPRINT( printf("From now on, parent will print out result of child\n"); );
        int sum=0;
        while((s=read(pipes[0],buf,100))){	//read returns 0 if file is closed
            sum+=s;
            //out[sum]=0x0;
            strncat(out,buf,100);
            out[sum]=0x0;
        }
        
        close(pipes[0]);

        DPRINT( printf("length of output: %d\n",sum); );
        out[sum]=0x0;
        DPRINT( printf("output:\n%s",out); );

        if(ctrlc) return false;

        wait(0x0);          //wait for child process to be done
        running=false;      //indicates that child process is finished

        //if(out[0]=='\0') return false;
            //int status;
            //wait(&status);
            //if(WEXITSTATUS(status) == 33){
        DPRINT( printf("\nerrmsg: %s\n",errmsg); );
        char* result=strstr(out,errmsg);
        if((result!=NULL)||checkloop){	//found the error string (crashes)
            count++;
            DPRINT( printf("true\n"); );
            return true;
        }
        else{
            DPRINT( printf("false\n"); );
            return false;
        }
    }
}

char* Reduce(char* crashInput, char* prgm, char** param, char* errmsg){
    int len=strlen(crashInput);
    int s = len - 1;
    //char* crash = crashInput;    
    char head[len];
    char mid[len];
    char tail[len];

    while(s > 0){
        printf("\nhead&tail\n");
        for(int i = 0; i < len-s+1; i++){
            //allocate memory for each part
            //head = (char*)malloc(sizeof(char)*i);
            //tail = (char*)malloc(sizeof(char)*(strlen(crash)-i-s)+1);
            
	    
            //copy the substrings to each part
            // if(i-1<0){
            //     head[0]='\0';		//check for empty string
            // }
            // else{
            //     strncpy(head,crashInput,i); // i+1이 아니라 i

            //     head[i+1] = '\0';
            // }
            
            // if(i+s>len-1){
            //     tail[0]='\0';
            // }
            // else{
            //     strncpy(tail,crashInput+i+s,len-s-i);
            //     tail[len-s-i]='\0';
            // }

            strncpy(head,crashInput,i); // i+1이 아니라 i
            head[i] = '\0';

            strncpy(tail,crashInput+i+s,len-s-i);
            tail[len-s-i]='\0';

            //strcat(head,tail);	//concatenate head and tail
            
            //if the string with the problem is found, reduce it further
            //(crashes)
            if(ExecutePrgm(strcat(head,tail),prgm,param,errmsg)) {
                printf("\nfound error string: %s...\nNow reducing further",strcat(head,tail));
                return Reduce(strcat(head,tail),prgm,param,errmsg);	
            }
            if(ctrlc) return crashInput;

        }
        
        printf("\nmid\n");
        for(int i = 0; i < len - s ;i++){
	        
            //find middle part
            //mid = (char*)malloc(sizeof(char)*s+1);
            strncpy(mid,crashInput+i,s);
            mid[s] = '\0';

            //try to find the error string in the middle part
            if(ExecutePrgm(mid,prgm,param,errmsg)) 
            {
                printf("\nfound error string: %s...\nNow reducing further",mid);
                return Reduce(mid,prgm,param,errmsg);
            }
            if(ctrlc) return crashInput;
        }
        s--;
    }


    printf("\nfound error string: %s\n",crashInput);
    printf("\ntotal crash occurred: %d\n", count);
    return crashInput;
}


void InputAnalysis(int argc, int paramlen, char* argv[], char* returnArr[4], char* tarArg[paramlen+1]) {
    //bool receivedParam = false;
    bool receivedProgram = false;

    if (argc < 7) {
        printf("Usage: %s -i inputPath -m errorString -o outputPath programToRun\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        DPRINT( printf("index: %d\n",i); );
        if (argv[i][0] == '-') {
            //receivedParam = true;
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
                tarArg[0]=returnArr[3];
                for(int k=1; k<paramlen+1; k++){
                    tarArg[k]=argv[i+k]; 	//storing parameters for target program
                    DPRINT( printf("%s\n",tarArg[k]); );
                }
                i+=paramlen;
                tarArg[paramlen+1]=NULL;
                receivedProgram = true;
            }
            //receivedParam = false;
        }
    }
    if (returnArr[3]==NULL){
    	printf("Missing argument for executable program parameter\n");
    	exit(1);
    }
}

void timeout(int sig) {	//when SIGALRM(timeout) happens
    if(sig==SIGALRM) puts("Time out!");
    //when timeout is detected, child process has to be killed since it falls into infinite loop
    //instead, checkloop is changed to true to let parent know whether it crashes or not
    checkloop=true;

    //since we have boolean variable 'running' for the status of child process
    // -> if running is false, it means that child process is still running when signalled.
    //so, we have to kill the running child for the cimin to proceed.
    if(running){
        kill(child,SIGKILL);        //kill running child using current child's pid
    }
}
void terminate(int sig) { //when SIGINT(control+c) happens
    if(sig==SIGINT) printf("\nCTRL+C is pressed!\nexit process..\n");
    //sets ctrlc to true since ctrl+c is pressed
    ctrlc=true;
    //since it has to exit, this case also needs to kill child if it is currently running
    if(running){
        kill(child,SIGKILL);
    }
}

int main(int argc, char* argv[]) {

    signal(SIGINT,terminate);	//when SIGINT, calls terminate to exit

    //since executePrgm runs recursively, it has to use pipe that is made in the each function call

    char* inputs[4] = { NULL, NULL, NULL, NULL };
    int paramlen=argc-8;
    char* tarArg[paramlen+2];

    DPRINT( printf("argc: %d, parameter length: %d\n",argc,paramlen); );
    InputAnalysis(argc, paramlen, argv, inputs, tarArg);

    DPRINT( printf("parameters of target program: "); );
    for(int i=0;i<paramlen+1;i++){
    	DPRINT( printf("%s, ",tarArg[i]); );
    }
    DPRINT( printf("\n"); );

    DPRINT( printf("Input path: %s\nError String: %s\nOutput path: %s\nProgram to be executed: %s\n", inputs[0], inputs[1], inputs[2], inputs[3]); );

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
        DPRINT( printf("content: %s\n", crashInput); );
        fclose(crash);
    }
    else{		//fread failed
        if(ferror(crash)) perror("Error reading crashing input file\n");
        else if(feof(crash)) perror("EOF found\n");
    }

    crashInput[crlen]='\0';

    DPRINT( printf("check if input is printed\n"); );

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

	//close(pipes[0]);		//close reading pipe
	//dup2(pipes[1],STDERR_FILENO);	//standard error -> writing pipe
	
	execv(inputs[3], tarArg);	//executing jsondump in child process
	perror("execvp failed");
	exit(1);
    }
    else{				//parent process
    	close(pipes[1]); 		//close writing pipe
	close(crin[0]);			//close reading pipe to write crashing input to pipe

	int check=write(crin[1],crashInput,crlen+1);	//write crashing input to writing pipe of crin
	//printf("written length of crash input: %d\n",check);
	if(check!=crlen+1) perror("write malfunctioning");
	close(crin[1]);				//since writing to crin is done, close writing pipe
	
	
    	while(s=read(pipes[0],buf,1023)){	//read returns 0 if file is closed
    	    //s=read(pipes[0],buf,1023);
	    //sum+=s;
	    buf[s+1]=0x0;
    	    printf(" stderr> %s",buf);
	}

	wait(0x0);			//wait until the child process is done
    }
*/

    char result[4096];
    DPRINT( printf("\n\nNow running by Reduce\n\n"); );
    strncpy(result,Reduce(crashInput,inputs[3],tarArg,inputs[1]),crlen);
    //int reslen=strlen(result);
    //result[reslen]='\0';
    printf("reduced crashing input is : %s\n",result);

    //need to store reduced crashing output to new file(inputs[2])
    FILE* store=fopen(inputs[2],"w");
    if(!store) {
        perror("error when writing to file");
        exit(1);
    }
    fprintf(store,"%s",result);
    fclose(store);

/*
    DPRINT( printf("\n\nNow running by ExecutePrgm\n\n"); );
    bool res=ExecutePrgm(crashInput,inputs[3],tarArg,inputs[1]);
    printf("result of ExecutePrgm: ");
    printf(res ? "true" : "false");
    printf("\n");
*/

    return 0;

}
