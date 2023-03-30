#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool Search(char* checkfor, char* inpString,int lng);

int main(int argc, char* argv[]) {
    char a[strlen(argv[1])];
    strcpy(a,argv[1]);
    
    printf("looking for: \"%s\" in: %s \n", argv[1], argv[2]);

    if(Search(a, argv[2],strlen(argv[1]))){
        printf("Found\n");
    }
    else{
        printf("Not Found\n");
    }
}

bool Search(char checkfor[], char* inpString, int lng) {
    char C = checkfor[0];
    char nextSearch[strlen(inpString)];
    int j = 0;

    bool found = false;

    for (int i = 0; i < strlen(inpString); i++)
    {
        if(found){
            //create new string with remaining
            nextSearch[j] = inpString[i];
            j++;
        }
        if(inpString[i] == C && !found){
            found = true;
        }
    }
    if(found && lng >= 1){
        char D[lng-1];
        for (int i = 0; i < lng-1; i++)
        {
            D[i] = checkfor[i+1];
        }
        if (lng == 1) return true;
        return Search(D, nextSearch,lng-1);
    }
    else{
        return false;
    }
    
}
