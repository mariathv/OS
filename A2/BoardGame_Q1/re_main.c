#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include<string.h>
#include <termios.h>
#include <signal.h>


#include "structQ1.h"

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void reset() {
    printf("\033[0m");
}

bool checkpos(int itemno, int itemsn, int x, int y, int itemsx[], int itemsy[], int totalp, int xcords[], int ycords[]){
   for(int i=0; i<totalp; i++){
        if(x==xcords[i] && y==ycords[i])
        return false;
   }
   for(int i=0; i<itemsn; i++){
    if (i==itemno)
    continue;
    if(x==itemsx[i] && y==itemsy[i])
    return false;
   }

   return true;
}

bool isplayercollide(int totalp, int xcords[], int ycords[], int curr){
    for(int i=0; i<totalp; i++){
        if(i==curr) continue;
        if(xcords[curr]==xcords[i] && ycords[curr]==ycords[i]){
            printf("collision\n");int sl;
            scanf("%d",&sl);
        return true;
        }
    }
    return false;
}

void generateBoard(int n, int itemsn, int turnof, int totalplayers, int xcords[] , int ycords[], int xitems[], int yitems[]) {
    printf("\033[1;32m");
    int i, j;
        int c=4; int k=0; // k -> total players
    for (i = 0; i < n; i++) {
        printf("\033[1;32m");
        if(i==0)
            printf("\033[1;97m");

        for (j = 0; j < n+1; j++) {
            if(i==0)
            printf("====");
            else
            printf("----");
        }
        printf("\n");
        for (j = 0; j < n; j++) {
            printf("\033[1;32m");
             bool flag=false;
            for(int k=0; k<totalplayers; k++){
                if(i==ycords[k] && j==xcords[k]){
                    //k++;
                    flag= true;
                    if(k==turnof){
                        printf("|\033[1;90m\033[1;107m %d ",k+1); reset();
                    }else
                    printf("|\033[1;90m\033[1;10%dm %d ", c, k+1); reset();
                    if(c==4) c=3;
                    else if(c==3) c=5;
                    else if(c==5) c=6;
                }
            }
            for(int k=0; k<itemsn; k++){
                if(i==yitems[k] && j==xitems[k]){
                    flag=true;
                    printf("|\033[1;91m O "); reset();
                }
            }
            if(!flag)
                printf("|   ");
        }
        printf("|\n");
    }
    printf("\033[1;97m");
    for (j = 0; j < n+1; j++) {
        printf("====");
    }
    printf("\n");
    reset();
}



int main(){
    //just a setup for reading key without pressing enter (NON BLOCKING READ)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ICANON; 
    newt.c_cc[VMIN] = 1;    
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); 


    srand(time(0));
    int randnum = rand() % 99 + 10;
    randnum*=2;
    int gridnum = (812) / randnum;
    gridnum=gridnum%25;

    if(gridnum<10)
    gridnum+=15;
    if(gridnum>18)
    gridnum-=5;
    
    int numofplayers=0;
 

    printf("grid size :: %d\n", gridnum);
    printf("enter number of players (max 5) ::");
    scanf("%d", &numofplayers);

    int xCords[numofplayers];
    int yCords[numofplayers];

    //assign xcords
    xCords[0]=1; xCords[1]=gridnum-2;
    if(numofplayers>=3)
        xCords[2]=1;
    if(numofplayers>=4)
        xCords[3]=gridnum-2;
    //assign ycords
    yCords[0]=1; yCords[1]=gridnum-2;
    if(numofplayers>=3){
        yCords[2] =  gridnum-2;
    }
    if(numofplayers>=4){
        yCords[3]=1;
    }

    int maintoplayers[2];
    int playerstomain[2];
    int playertoplayer[2];
    pid_t pids[4];

    if(pipe(playertoplayer)==-1){
        perror("pipe::"); exit(EXIT_FAILURE);
    }
    if(pipe(maintoplayers)==-1){
        perror("pipe::"); exit(EXIT_FAILURE);
    }
    if(pipe(playerstomain)==-1){
        perror("pipe::"); exit(EXIT_FAILURE);
    }
    for(int i=0 ; i<numofplayers; i++){
        pid_t pid = fork();
        if(pid == -1){
            perror("fork::"); exit(EXIT_FAILURE);
        }
        if(pid>0)
            pids[i]=pid;
        if(pid == 0){
            pipemsg playeresend; pipemsg playerget;
            close(playerstomain[0]);
            close(maintoplayers[1]);
            int turnof=1;
            while(true){
                if(turnof == i+1){
    
                read(maintoplayers[0], &playerget, sizeof(playerget));
                if(playerget.isMoveDone==false && playerget.turnof==i+1){
                    char keypressed;
                    bool correctkey=false;
                    do{
                        int bytes_read = read(STDIN_FILENO, &keypressed, 1); // Non-blocking read
                        if (bytes_read == 1) {
                        switch (keypressed)
                        {
                        case 'w':
                        if(yCords[i]==0) break;
                        yCords[i]--; correctkey=true;
                            break;
                        case 's':
                        if(yCords[i]==gridnum-1) break;
                        yCords[i]++;
                        correctkey=true;
                            break;
                        case 'a':
                        if(xCords[i]==0) break;
                        xCords[i]--;
                        correctkey=true;
                            break;
                        case 'd':
                        if(xCords[i]==gridnum-1) break;
                        xCords[i]++;
                        correctkey=true;
                            break;
                        }
                            
                        }
                        if(isplayercollide(numofplayers,xCords,yCords,i)){
                                correctkey=false;
                        }
                    }while(!correctkey);

                
                    playeresend.isMoveDone=true;
                    playeresend.xPos=xCords[i];
                    playeresend.yPos=yCords[i];
                    playeresend.turnof=i+1;
                    write(playerstomain[1], &playeresend, sizeof(playeresend));
                    for(int i=0; i<numofplayers-1; i++)
                    write(playertoplayer[1], "done", 5);
                    turnof++;
                    if(turnof>numofplayers) turnof=1;
                    sleep(1);
                }
                }else{
                    char buffer[5];
                    read(playertoplayer[0], &buffer, 5);
                    turnof++;
                    if(turnof>numofplayers) turnof=1;
                }
            }
        }
    }
    //MAIN PROCESS

    //at a time (numofplayers+1) items will be generated
    //int itemsn=numofplayers+1;
    int itemsn = gridnum/3;
    int itemxPos[itemsn];
    int itemyPos[itemsn];
    for(int i=0; i<itemsn; i++){
        itemxPos[i]=-1; itemyPos[i]=-1;
    }
    bool alrexsflag=false; //flag to check if item is generated a right place
    int score[numofplayers];
    for(int i=0; i<numofplayers; i++)
    score[i]=0;


    pipemsg mainsend; pipemsg mainget;
    mainsend.isMoveDone=false; mainget.isMoveDone=false;
    close(maintoplayers[0]);
    close(playerstomain[1]);
    int turnof = 0; bool firstmove = true;
    int itemscollected=-1;
    while(true){
        //printf("reach\n");
        if(firstmove || mainget.turnof==turnof && mainget.isMoveDone == true){
            mainget.isMoveDone=false;
            if(!firstmove){
                xCords[mainget.turnof-1]=mainget.xPos;
                yCords[mainget.turnof-1]=mainget.yPos;
                for(int i=0; i<itemsn; i++){
                    if(mainget.xPos == itemxPos[i] && mainget.yPos == itemyPos[i]){
                        itemxPos[i]=-1; itemyPos[i]=-1;
                        do{
                            alrexsflag=false;
                            itemxPos[i]=rand()%(gridnum-1);
                            itemyPos[i]=rand()%(gridnum-1);
                            if(checkpos(i, itemsn, itemxPos[i], itemyPos[i],itemxPos, itemyPos,numofplayers,xCords, yCords)==false){
                                alrexsflag=true;
                            }
                        }while(alrexsflag);
                        itemscollected++;
                        score[mainget.turnof-1]++;
                        if(score[mainget.turnof-1]==10){
                            printf("\033[1;94mPLAYER %d \033[1;92mWON!\n", mainget.turnof);
                            for(int i=0; i<numofplayers;i++){
                                kill(pids[i], SIGKILL);
                            }
                            exit(EXIT_SUCCESS);
                        }
                        break;
                    }
                }
            }
            if(itemscollected==-1){
                itemscollected=0;
                for(int i=0; i<itemsn; i++){
                    do{
                        alrexsflag=false;
                        itemxPos[i]=rand()%(gridnum-1);
                        itemyPos[i]=rand()%(gridnum-1);
                        if(checkpos(i, itemsn, itemxPos[i], itemyPos[i],itemxPos, itemyPos,numofplayers,xCords, yCords)==false){
                            alrexsflag=true;
                        }
                    }while(alrexsflag);
                }
            }
            turnof++;
            if(turnof>numofplayers) turnof=1;
            system("clear"); printf("|");
            for(int i=0; i<numofplayers; i++){
                printf("\033[1;92m  Player %d : \033[1;35m%d \033[1;92m|", i+1, score[i]);reset();
            }
            printf("\n---\n\033[1;94mPLAYER \033[1;95m%d \033[1;94mTURN::\n\n", turnof);reset();
            firstmove= false;
            mainsend.turnof=turnof;
            generateBoard(gridnum, itemsn, mainsend.turnof-1, numofplayers,xCords,yCords, itemxPos, itemyPos);
            write(maintoplayers[1], &mainsend, sizeof(mainsend));
        }
        read(playerstomain[0], &mainget, sizeof(pipemsg));
    }


}
