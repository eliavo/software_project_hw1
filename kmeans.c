#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_d(FILE*);
double** parse_input(FILE*);
void free_data(double**);
double** set_initial_means(double**, int);
void copy_means(double**, double**);
void update_means(double**, double**);
int calc_change(double**, double**);

int d;
const double e = 0.001;

int main(int argc, char* argv[]){
    int K, max_iter;
    FILE *input, *output;
    double **data, **means, **copy, change;
    int i;

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
    means = set_initial_means(data, K);
    copy = (double**)malloc(sizeof(double*)*K);
    for (i=0; i<K; copy[i++] = (double*)malloc(sizeof(double)*d));

    do {
        copy_means(means, copy);
        update_means(data, means);
        change = calc_change(means, copy);
        max_iter--;
    } while (change>e && max_iter>0);

    printf("%d %d %d %p\n",K, max_iter, d, (void*)data);

    free_data(data);
    free_data(means);
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
    int i;
    
    for (i=0; data[i]!=NULL; i++)
        free(data[i]);
    
    free(data);
}

double** set_initial_means(double **data, int K) {
    double **means;
    int i;

    means = (double**)malloc(sizeof(double*)*(K+1));
    for (i=0; i<K; i++) {
        means[i] = (double*)malloc(sizeof(double)*d);
        memcpy(means[i], data[i], sizeof(double)*d);
    }

    means[K] = NULL;
    return means;
}

void copy_means(double** means, double** copy) {
    int i;
    for (int i=0; means[i] != NULL; i++)
        memcpy(copy, means, sizeof(double)*d);
}