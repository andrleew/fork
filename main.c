#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

void read_file(FILE * file ,int * array){
    int c;
    while ((c = fgetc(file)) != EOF) array[c]++;
}

void write_file(FILE * file, int * array){
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < 256; i++){
        for (int j = 0; j < array[i]; j++)
        fprintf(file, "%c", i);
    }
}

void ok(){printf("ok\n");}

int main(){
    printf("enter first file\n");
    char str[256];
    scanf("%s", str);
    FILE * file1 = fopen(str, "r+");
    if (!file1){
        perror("can't open first file");
        exit(1);
    }
    printf("enter second file\n");
    scanf("%s", str);
    FILE * file2 = fopen(str, "r+");
    if (!file2){
        perror("can't open second file");
        exit(2);
    }
    int descriptors[2];
    pipe(descriptors);
    int * array1 = (int*)malloc(256* sizeof(int));
    int * array2 = (int*)malloc(256* sizeof(int));
    for (int i = 0; i < 256; i++){
        array1[i] = 0;
        array2[i] = 0;
    }
    switch (fork()){
        case 0:
            switch(fork()){
                case 0:
                    read_file(file1, array1);
                    write_file(file1, array1);
                    for (int i = 0; i < 256; i++)
                        write(descriptors[1], &array1[i], sizeof(int));
                    exit(0);
                default:
                    read_file(file2, array2);
                    write_file(file2, array2);
                    wait(NULL);
                    for (int i = 0; i < 256; i++){
                        read(descriptors[0], &array1[i], sizeof(int));
                        array2[i] += array1[i];
                        write(descriptors[1], &array2[i], sizeof(int));
                    }
                    exit(0);
            }
        default:
        wait(NULL);
    }
    for (int i = 0; i < 256; i++){
        read(descriptors[0], &array1[i], sizeof(int));
    }
    write_file(stdout, array1);
    printf("\n");
    free(array1);
    free(array2);
    fclose(file1);
    fclose(file2);
    return 0;
}