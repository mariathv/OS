#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

void cdfver();
void cdfhelp();
void cdfmkdir(int argc, char *argv[]);
void cdftouchw(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("! Error: No arguments specified.\n");
        printf("> Use 'cdf --help' command to see the list of supported arguments.\n");
        exit(1);
    }

    if (strcmp(argv[1], "--version") == 0)  {
        cdfver();
    } else if (strcmp(argv[1], "--help") == 0) {
        cdfhelp();
    } else if (strcmp(argv[1], "-d") == 0) {
        cdfmkdir(argc, argv);
    } else if (strcmp(argv[1], "-f") == 0) {
        cdftouchw(argc, argv);
    } else if (strcmp(argv[1], "-t") == 0) {
        printf("! Error: File name missing.\n");
        printf("> Use '-f' option to specify filename.\n");
        exit(1);
    } else {
        printf("! Error: Unknown command.\n");
        printf("> Use 'cdf --help' command to see the list of supported arguments.\n");
        exit(1);
    }
}

void cdfver() {
    printf(":: cdf v1.0\n");
}

void cdfhelp(){
    printf("     --version                       prints version of cdf\n");
    printf("     -d directoryName(s)             creates a directory(s) in the current directory\n");
    printf("     -f filename                     creates an empty file with the given name\n");
    printf("     -f filename -t text             creates an empty file with the given name and inputs text\n");
}

void cdfmkdir(int argc, char *argv[]){
    for (int i = 2; i < argc; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("! Error - fork() failure");
            exit(1);
        } else if (pid == 0) {
            // Child process creates directory while parent waits
            if (mkdir(argv[i], 0777) == -1) {
                perror("mkdir");
                exit(1);
            } else {
                printf("> '%s' directory created successfully\n", argv[i]);
                exit(0);
            }
        }
    }
    // Parent process waits for all child processes to finish
    for (int i = 2; i < argc; i++) {
        wait(NULL);
    }
}

void cdftouchw(int argc, char *argv[]){
    if (argc < 3) {
        printf("! Error: Filename missing.\n");
        exit(1);
    }

    FILE *file = fopen(argv[2], "w");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    } else {
        if (argc >= 5 && strcmp(argv[3], "-t") == 0) {
            for (int i = 4; i < argc; i++) {
                fprintf(file, "%s ", argv[i]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
        exit(0);
    }
}