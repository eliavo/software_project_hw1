#include <stdio.h>
#include <stdlib.h>

int check_valid_k(double**, int);
int get_d(FILE*);
double** parse_input(FILE*);
void free_data(double**);
double** set_initial_means(double**, int);
void copy_means(double**, double**);
void update_means(double**, double**, double**, int*);
double calc_dist(double*, double*);
double calc_change(double**, double**);
void write_res(FILE*, double**);

int d;
const double e = 0.001;

int main(int argc, char* argv[]){
    int K, max_iter;
    FILE *input, *output;
    double **data, **means, **copy;
    int *counters;

    if (argc<4) {
        perror("Invalid input!\n");
        return 1;
    }

    K = atoi(argv[1]);
    if (K<=1) {
        perror("Invalid input!\n");
        return 1;
    }
    if (argc == 4) {
        max_iter = 200;
        input = fopen(argv[2], "r");
        output = fopen(argv[3], "w");
    }
    else {
        max_iter = atoi(argv[2]);
        if (max_iter<=0) {
            perror("Invalid input!\n");
            return 1;
        }

        input = fopen(argv[3], "r");
        output = fopen(argv[4], "w");
    }

    d = get_d(input);
    data = parse_input(input);
    if (!check_valid_k(data, K)) {
        perror("Invalid input!\n");
        free_data(data);
        fclose(input);
        fclose(output);
        return 1;
    }
    means = set_initial_means(data, K);
    copy = set_initial_means(data, K);
    counters = (int*)malloc(sizeof(int)*K);

    do {
        copy_means(copy, means);
        update_means(data, means, copy, counters);
        max_iter--;

    } while (calc_change(copy, means)>e*e && max_iter>0);

    write_res(output, means);

    free_data(data);
    free_data(means);
    free_data(copy);
    free(counters);
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

int check_valid_k(double** data, int K) {
    int i;
    for (i=0; data[i] != NULL; i++);
    if (K>=i) return 0;
    return 1;
}

double** parse_input(FILE *input) {
    char c, buffer[128];
    int lines, i, j, offset;
    double** data;

    for (lines=c=0; (c=fgetc(input))!=EOF; lines += (c=='\n'));
    
    data = (double**)malloc(sizeof(double*)*(lines+1));
    rewind(input);

    for(i=0; i<lines; i++) {
        data[i] = (double*)malloc(sizeof(double)*d);
        for (j=0; j<d; j++) {
            offset = fscanf(input, "%128[^,^\n]", buffer);
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
    int i, j;

    means = (double**)malloc(sizeof(double*)*(K+1));
    for (i=0; i<K; i++) {
        means[i] = (double*)malloc(sizeof(double)*d);
        for (j=0; j<d; j++)
            means[i][j] = data[i][j];
    }

    means[K] = NULL;
    return means;
}

void copy_means(double** target, double** input) {
    int i, j;
    for (i=0; input[i] != NULL; i++)
        for (j=0; j<d; j++)
            target[i][j] = input[i][j];
}

void update_means(double** data, double** means, double** copy, int* counters) {
    int i, j, min_dist_element;
    double min_dist, dist;
    
    for (i=0; means[i] != NULL; i++) {
        for (j=0; j<d; j++)
            means[i][j] = 0;
        counters[i] = 0;
    } /* sets copy, counters to 0. copy will be added to and devided with counters
    to get the mean of each cluster */

    for (i=0; data[i] != NULL; i++) {
        min_dist_element = 0;
        min_dist = calc_dist(data[i], copy[0]);
        for (j=1; copy[j] != NULL; j++) {
            dist = calc_dist(data[i], copy[j]);
            if (dist<min_dist) {
                min_dist = dist;
                min_dist_element = j;
            }
        }

        for (j=0; j<d; j++)
            means[min_dist_element][j] += data[i][j];

        counters[min_dist_element]++;
    }

    for (i=0; means[i] != NULL; i++)
        for (j=0; j<d; j++)
            means[i][j] = means[i][j]/counters[i];
}

double calc_dist(double* v, double* s) {
    double dist, difference;
    int i;

    dist = 0;
    for (i=0; i<d; i++) {
        difference = v[i]-s[i];
        dist += difference*difference;
    }

    return dist;
}

double calc_change(double** prev, double** curr) {
    double change, dist;
    int i;
    
    change = calc_dist(prev[0], curr[0]);
    for (i=1; prev[i] != NULL; i++) {
        dist = calc_dist(prev[i], curr[i]);

        if (change<dist) change = dist;
    }
    return change;
}

void write_res(FILE* output, double** means) {
    int i, j;

    
    for (i=0; means[i] != NULL; i++) {
        for (j=0; j<d-1; j++) {
            fprintf(output, "%.4f,", means[i][j]);
        }
        fprintf(output, "%.4f\n", means[i][d-1]);
    }
}