#include <stdio.h>
#include <stdlib.h>

int get_d(FILE*);

int d;

int main(int argc, char* argv[]){
    int K, max_iter;
    FILE *input, *output;

    if (argc<4) {
        perror("Invalid input!\n");
        return 1;
    }

    K = atoi(argv[1]);
    if (argc == 4) {
        max_iter = 200;
        input = fopen(argv[2], "r");
        output = fopen(argv[3], "w");
    }
    else {
        max_iter = atoi(argv[2]);
        input = fopen(argv[3], "r");
        output = fopen(argv[4], "w");
    }

    d = get_d(input);

    fclose(input);
    fclose(output);
    return 0;
}

int get_d(FILE *input) {
    int d;
    char c;
    
    d = 1;
    while ((c=(char)fgetc(input)) != '\n')
        if (c == ',')
            d++;

    rewind(input);
    return d;
}