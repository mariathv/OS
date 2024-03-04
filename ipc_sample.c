#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int pipeParentToChild[2];
    int pipeChildToParent[2];
    pid_t cpid;
    int array[20];

    if (pipe(pipeParentToChild) == -1 || pipe(pipeChildToParent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {   
        close(pipeParentToChild[1]);  
        close(pipeChildToParent[0]);  
        int sum=0;
        int gett;
       
        for(int i=0; i<10; i++){
        read(pipeParentToChild[0], &gett, 1);
        sum+=gett;
        }

        printf("\n> Child Sum :: (last 10 Integers) :     %d \n", sum);

        write(pipeChildToParent[1], &sum, 1);

        close(pipeParentToChild[0]);
        close(pipeChildToParent[1]);
        exit(EXIT_SUCCESS);

    } else {           

        printf("Enter 20 Integers:: \n");
        for(int i=0; i<20; i++){
            scanf("%d", &array[i]);
        }

        close(pipeParentToChild[0]);  
        close(pipeChildToParent[1]);  

        int tsum=0;
        for(int i=0; i<20; i++){
            if(i>=10)
            write(pipeParentToChild[1], &array[i], 1); 
            else
            tsum+=array[i];
        }
        int csum;
      
        read(pipeChildToParent[0], &csum, 1);
        printf("> Parent Sum :: (first 10 Integers) :     %d \n", tsum);

        tsum+=csum;

        close(pipeParentToChild[1]);
        close(pipeChildToParent[0]);
        printf("--------------------\n");
        printf("total sum : %d \n", tsum);
        printf("--------------------\n");
        wait(NULL);        
        exit(EXIT_SUCCESS);
    }
}
