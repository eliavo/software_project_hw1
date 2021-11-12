#include <stdio.h>
#include <stdlib.h>

int get_d(FILE*);
double** parse_input(FILE*);
void free_data(double**);

int d;

int main(int argc, char* argv[]){
    int K, max_iter;
    FILE *input, *output;
    double** data;

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
    
    data = parse_input(input);

    printf("%d %d %d %p\n",K, max_iter, d, (void*)data);

    free_data(data);
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

double** parse_input(FILE *input) {
    char c, buffer[128];
    int lines, i, j, offset;
    double** data;

    for (lines=c=0; (c=fgetc(input))!=EOF; lines += (c=='\n'));
    
    data = (double**)malloc(sizeof(double*)*lines);
    rewind(input);

    for(i=0; i<lines; i++) {
        data[i] = (double*)malloc(sizeof(double)*d);
        for (j=0; j<d; j++) {
            offset = fscanf(input, "%[^,^\n]", buffer);
            fseek(input, offset, SEEK_CUR);
            data[i][j] = atof(buffer);
        }
    }

    data[lines] = NULL;
    return data;
}

void free_data(double** data) {
    double *entry;
    int i;
    
    i = 0;
    for (entry=data[i++]; entry!=NULL; entry=data[i++])
        free(entry);
    
    free(data);
}