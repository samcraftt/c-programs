#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void print_usage(char *msg);

/* return an array that has the number of lines, words, and characters in filename */
/* should pass in a filename of "" to indicate to read from stdin */
int *get_counts(char *filename);

/* print the indicated counts for file name */
/* show should be an array of three ints that indicates if the number of lines,
 * words, and characters should be printed */
/* count is an array of the three counts */
/* name is the name to print after the counts */
void print_counts(int *show, int *count, char *name);

int main(int argc, char **argv)
{
    int first_filename = 1; // index of the first filename 
    int use_stdin = 0; // turns to 1 to indicate that we're reading from standard input
    int to_display[] = {0, 0, 0}; // indicates which counts are to be printed
    if (argc == 1)
    {
        use_stdin = 1;
    }
    // iterate through command line, break when first filename is found
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            first_filename = i;
            break;
        }
        if (i == argc-1)
        {
            use_stdin = 1;
        }
        if (strcmp(argv[i], "-l") != 0 && strcmp(argv[i], "-w") != 0  && strcmp(argv[i], "-c") != 0)
        {
            print_usage("invalid argument");
        }
        if (strcmp(argv[i], "-l") == 0)
        {
            to_display[0] = 1;
        }
        if (strcmp(argv[i], "-w") == 0)
        {
            to_display[1] = 1;
        }
        if (strcmp(argv[i], "-c") == 0)
        {
            to_display[2] = 1;
        }
    }
    if (to_display[0] == 0 && to_display[1] == 0 && to_display[2] == 0)
    {
        to_display[0] = 1;
        to_display[1] = 1;
        to_display[2] = 1;
    }
    if (use_stdin == 1)
    {
        int *three_counts = get_counts("");
        if (three_counts == NULL)
        {
            printf("Failed to get counts from standard input\n");
            return 1;
        }
        print_counts(to_display, three_counts, "");
        free(three_counts);
    }
    else
    {
        int totals[] = {0, 0, 0};
        for (int i = first_filename; i < argc; i++)
        {
            int *three_counts = get_counts(argv[i]);
            if (three_counts == NULL)
            {
                continue;
            }
            print_counts(to_display, three_counts, argv[i]);
            totals[0] += three_counts[0];
            totals[1] += three_counts[1];
            totals[2] += three_counts[2]; 
            free(three_counts);
        } 
        if (argc > first_filename + 1)
        {
            for (int i = 0; i < 3; i++)
            {
                if (to_display[i] == 1)
                {
                    printf("%8d ", totals[i]);
                }
            }
            printf("total\n");
        }
    }
    return 0;
}

void print_usage(char *msg)
{
    if (msg != NULL)
    {
        printf("%s\n", msg);
    }
    printf("\nUsage: wc [-l] [-w] [-c] [FILES...]\n");
    printf("where:\n");
    printf("       -l    prints the number of lines\n");
    printf("       -w    prints the number of words\n");
    printf("       -c    prints the number of characters\n");
    printf("       FILES if no files are given, then read\n");
    printf("             from standard input\n");
    exit(1);
}

int *get_counts(char *filename)
{
    if (filename == NULL)
    {
        printf("Null filename provided to get_counts\n");
        return NULL;
    }
    int *counts = (int *)calloc(3, sizeof(int));
    if (counts == NULL)
    {
        printf("Memory allocation failed in get_counts\n");
        return NULL;
    }
    int fd;
    char c;
    ssize_t successfully_read;
    int in_whitespace = 1;
    if (strcmp(filename, "") == 0)
    {
        fd = 0;
    }
    else
    {
        fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
            perror(filename);
            free(counts);
            return NULL;
        }
    }
    successfully_read = read(fd, &c, 1);
    if (successfully_read == -1)
    {
        perror(filename);
        free(counts);
        if (fd != 0)
        {
            close(fd);
        }
        return NULL;
    }
    while (successfully_read > 0)
    {
        if (c == '\n')
        {
            counts[0]++;
        }
        if (isspace(c) && !in_whitespace)
        {
            counts[1]++;
            in_whitespace = 1;
        }
        if (!isspace(c) && in_whitespace)
        {
            in_whitespace = 0;
        }
        counts[2]++;
        successfully_read = read(fd, &c, 1);
    }
    if (successfully_read == -1)
    {   
        perror(filename);
        free(counts);
        if (fd != 0)
        {   
            close(fd);
        }   
        return NULL;
    }
    if (!in_whitespace)
    {
        counts[1]++;
    }
    if (fd != 0)
    {
        close(fd);
    }
    return counts;
}

void print_counts(int *show, int *count, char *name)
{
    if (show == NULL || count == NULL || name == NULL)
    {
        printf("Null argument provided to print_counts\n");
        return;
    }
    for (int i = 0; i < 3; i++)
    {
        if (show[i])
        {
            printf("%8d ", count[i]);
        }
    }
    printf("%s\n", name);
}
