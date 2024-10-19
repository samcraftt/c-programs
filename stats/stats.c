#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

void usage();
int is_valid_int(char *s);
int *generate_population(int size, int lower, int upper);
void get_stats(int *a, int size, int *min, int *max, double *mean, double *stddev);

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("incorrect number of arguments\n");
        usage();
    }
    for (int i = 1; i < argc; i++)
    {
        if (is_valid_int(argv[i]) == 0)
        {
            printf("all arguments must be integers\n");
            usage();
        }
    }
    if (atoi(argv[1]) < 1)
    {
        printf("samples must be a positive integer\n");
        usage();
    }
    if (atoi(argv[2]) < 1)
    {
        printf("population must be a positive integer\n");
        usage();
    }
    if (atoi(argv[4]) < atoi(argv[3]))
    {
        printf("upperbound must be >= lowerbound\n");
        usage();
    }
    srand(time(NULL));
    int num_samples = atoi(argv[1]);
    int population_size = atoi(argv[2]);
    int lower_bound = atoi(argv[3]);
    int upper_bound = atoi(argv[4]);
    for (int i = 1; i <= num_samples; i++)
    {
        int minimum;
        int *min = &minimum;
        int maximum;
        int *max = &maximum;
        double mean_value;
        double *mean = &mean_value;
        double standard_deviation;
        double *stddev = &standard_deviation;
        int *p = generate_population(population_size, lower_bound, upper_bound);
        if (p == NULL)
        {
            return 1;
        }
        get_stats(p, population_size, min, max, mean, stddev);
        free(p);
        printf("Sample %i: min=%i, max=%i, mean=%g, stddev=%g\n", i, *min, *max, *mean, *stddev);
    } 
    return 0;
}

void usage()
{
    printf("\nusage: stats samples population lowerbound upperbound\n");
    printf("       samples: number of samples\n");
    printf("       population: number of random values to generate in each sample\n");
    printf("       lowerbound: bottom of random number range\n");
    printf("       upperbound: top of random number range\n");
    exit(1);
}

int is_valid_int(char *s)
{
    if (s == NULL)
    {
        return -1;    
    }

    if (*s == '-')
    {
        ++s;
    }
    while (*s != '\0')
    {
        if (!isdigit(*s))
        {
            return 0;
        }
        ++s;
    }
    return 1;
}

int *generate_population(int size, int lower, int upper)
{
    int *array = (int *)calloc(size, sizeof(int));
    if (array == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < size; i++)
    {
        array[i] = (rand() % (upper-lower+1)) + lower;
    }
    return array;
}

void get_stats(int *a, int size, int *min, int *max, double *mean, double *stddev)
{
    if (a == NULL || min == NULL || max == NULL || mean == NULL || stddev == NULL)
    {
        return;
    }
    *min = a[0];
    *max = a[0];
    double sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        if (a[i] < *min)
        {
            *min = a[i];
        }
        if (a[i] > *max)
        {
            *max = a[i];
        }
        sum += a[i];
    }
    *mean = sum/size;
    sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        sum += (a[i] - *mean) * (a[i] - *mean);
    }
    *stddev = sqrt(sum/size);
}
