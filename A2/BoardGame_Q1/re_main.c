#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include<string.h>
#include <termios.h>


#include "structQ1.h"

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void reset() {
    printf("\033[0m");
}

bool checkpos(int itemno, int x, int y, int itemsx[], int itemsy[], int totalp, int xcords[], int ycords[]){
   for(int i=0; i<totalp; i++){
        if(x==xcords[i] && y==ycords[i])
        return false;
   }
   for(int i=0; i<3; i++){
    if (i==itemno)
    continue;
    if(x==itemsx[i] && y==itemsy[i])
    return false;
   }

   return true;
}

void generateBoard(int n, int totalplayers, int xcords[] , int ycords[]) {

    //for(int i=0; i<totalplayers; i++){
    //    printf("player %d xcord:%d  ycord:%d\n",i+1,xcords[i], ycords[i]);
    //}
    int i, j;
        int c=4; int k=0; // k -> total players
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            printf("----");
        }
        printf("-\n");
        for (j = 0; j < n; j++) {
             bool flag=false;
            for(int k=0; k<totalplayers; k++){
                if(i==ycords[k] && j==xcords[k]){
                    k++;
                    flag= true;
                    printf("|\033[1;9%dmXXX", c); reset();
                    c++;
                    if(c==6) c++;
                    if(c==7) c=2;
                }
            }
            for(int k=0; k<3; k++){
                
            }
            if(!flag)
                printf("|   ");

        }
        printf("|\n");
    }
    for (j = 0; j < n; j++) {
        printf("----");
    }
    printf("-\n");
}



int main(){
    //just a setup for reading key without pressing enter (NON BLOCKING READ)
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt); // Get current terminal settings
    newt = oldt;
    newt.c_lflag &= ~ICANON; // Disable canonical mode (line buffering)
    newt.c_cc[VMIN] = 1;     // Set minimum number of characters for read to return (1)
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); 


    srand(time(0));
    int randnum = rand() % 99 + 10;
    randnum*=2;
    int gridnum = (812) / randnum;
    gridnum=gridnum%25;

    // if(gridnum<10)
    // gridnum+=15;
    // if(gridnum>20)
    // gridnum-=5;
    
    int numofplayers=0;
    gridnum = 7;

    printf("grid size :: %d\n", gridnum);
    printf("enter number of players (max 5) ::");
    scanf("%d", &numofplayers);
    //clear_input_buffer();

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
        if(pid == 0){
            pipemsg playeresend; pipemsg playerget;
            close(playerstomain[0]);
            close(maintoplayers[1]);
            int turnof=1;
            while(true){
                //printf("child %d :: turn of %d\n", i+1, turnof);
                if(turnof == i+1){
                //printf("child %d reading\n", i+1);
                read(maintoplayers[0], &playerget, sizeof(playerget));
               // printf("child %d read TURN OF %d, isMOVEDONE %d\n", i+1,playerget.turnof, playerget.isMoveDone);
                if(playerget.isMoveDone==false && playerget.turnof==i+1){
                    //printf("> PLAYER %d MOVING\n", i+1);
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
                        if(xCords[i]==gridnum-1);
                        xCords[i]++;
                        correctkey=true;
                            break;
                        }
                        }
                    }while(!correctkey);

                
                    playeresend.isMoveDone=true;
                    playeresend.xPos=xCords[i];
                    playeresend.yPos=yCords[i];
                    playeresend.turnof=i+1;
                    write(playerstomain[1], &playeresend, sizeof(playeresend));
                    for(int i=0; i<numofplayers-1; i++)
                    write(playertoplayer[1], "done", 5);
                    //printf("done sending xpos %d and ypos %d\n", playeresend.xPos, playeresend.yPos);
                    //turnof++;
                    turnof++;
                    if(turnof>numofplayers) turnof=1;
                    sleep(1);
                    //printf("child %d WROTE, turnof %d, ismovedone %d\n", i+1, playeresend.turnof, playeresend.isMoveDone);
                }
                }else{
                    char buffer[5];
                    //printf("child %d waiting for another child\n",i+1);
                    read(playertoplayer[0], &buffer, 5);
                    turnof++;
                    if(turnof>numofplayers) turnof=1;
                    //printf("child %d done waiting for another child\n", i+1);
                }
                //printf("child %d",i+1);
            }
        }
    }
    //MAIN PROCESS

    //at a time 3 items will be generated
    int itemxPos[3]={-1,-1,-1};
    int itemyPos[3]={-1,-1,-1};
    bool alrexsflag=false; //flag to check if item is generated a right place
    for(int i=0; i<3; i++){
        do{
        alrexsflag=false;
        itemxPos[i]=rand()%(gridnum-1);
        itemyPos[i]=rand()%(gridnum-1);
        if(checkpos(i, itemxPos[i], itemyPos[i],itemxPos, itemyPos,numofplayers,xCords, yCords)==false){
            alrexsflag=true;
        }
        }while(alrexsflag);
    }


    pipemsg mainsend; pipemsg mainget;
    mainsend.isMoveDone=false; mainget.isMoveDone=false;
    close(maintoplayers[0]);
    close(playerstomain[1]);
    int turnof = 0; bool firstmove = true;
    while(true){
        //printf("reach\n");
        if(firstmove || mainget.turnof==turnof && mainget.isMoveDone == true){
            mainget.isMoveDone=false;
            if(!firstmove){
            xCords[mainget.turnof-1]=mainget.xPos;
            yCords[mainget.turnof-1]=mainget.yPos;
            }
            turnof++;
            if(turnof>numofplayers) turnof=1;
            system("clear");
            printf("PLAYER %d TURN::\n", turnof);
            firstmove= false;
            mainsend.turnof=turnof;
            generateBoard(gridnum,numofplayers,xCords,yCords);
            //for(int i=0; i<numofplayers;i++)
            write(maintoplayers[1], &mainsend, sizeof(mainsend));
            //printf("main wrote to child turnof :: %d, ismovedone:: %d\n", mainsend.turnof, mainsend.isMoveDone);
        }
       // printf("main waiting for child %d to write\n", mainsend.turnof);
        read(playerstomain[0], &mainget, sizeof(pipemsg));
        //printf("parent");
        //sleep(1);
    }


}

//DEBUGGING TIPS
//FOR LOOP FOR READING AND WRITING TO ALL CHILD?
//DOUBLE CHECK PASSING OF X AND Y LOOP