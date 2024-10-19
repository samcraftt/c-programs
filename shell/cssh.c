#define _POSIX_C_SOURCE 200809L // required for strdup() on cslab
#define _DEFAULT_SOURCE // required for strsep() on cslab
#define _BSD_SOURCE // required for strsep() on cslab

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_ARGS 32

char **get_next_command(size_t *num_args)
{
    // print the prompt
    printf("cssh$ ");

    // get the next line of input
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    if (ferror(stdin))
    {
        perror("getline");
        exit(1);
    }
    if (feof(stdin))
    {
        return NULL;
    }

    // turn the line into an array of words
    char **words = (char **)malloc(MAX_ARGS * sizeof(char *));
    int i = 0;

    char *parse = line;
    while (parse != NULL)
    {
        char *word = strsep(&parse, " \t\r\f\n");
        if (strlen(word) != 0)
        {
            words[i++] = strdup(word);
        }
    }
    *num_args = i;
    for (; i < MAX_ARGS; ++i)
    {
        words[i] = NULL;
    }

    // all the words are in the array now, so free the original line
    free(line);

    return words;
}

void free_command(char **words)
{
    for (int i = 0; i < MAX_ARGS; ++i)
    {
        if (words[i] == NULL)
        {
            break;
        }
        free(words[i]);
    }
    free(words);
}

int handle_redirection(char **command_line_words, size_t *num_args)
{
    int input_fd = -1, output_fd = -1;
    int input_count = 0, output_count = 0;
    int redirection_type = 0; // 1 for >, 2 for >>

    for (size_t i = 0; i < *num_args; ++i)
    {
        if (command_line_words[i] == NULL)
            break;

        if (strcmp(command_line_words[i], "<") == 0)
        {
            if (++input_count > 1)
            {
                fprintf(stderr, "Error! Can't have two <'s!\n");
                return -1;
            }
            input_fd = open(command_line_words[i + 1], O_RDONLY);
            if (input_fd == -1)
            {
                perror("open");
                return -1;
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
            // Shift the remaining arguments
            for (size_t j = i; j + 2 < *num_args; ++j)
            {
                command_line_words[j] = command_line_words[j + 2];
            }
            *num_args -= 2;
            i--;
        }
        else if (strcmp(command_line_words[i], ">") == 0)
        {
            if (++output_count > 1 || redirection_type == 2)
            {
                fprintf(stderr, "Error! Can't have two >'s or >>'s!\n");
                return -1;
            }
            redirection_type = 1;
            output_fd = open(command_line_words[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1)
            {
                perror("open");
                return -1;
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            // Shift the remaining arguments
            for (size_t j = i; j + 2 < *num_args; ++j)
            {
                command_line_words[j] = command_line_words[j + 2];
            }
            *num_args -= 2;
            i--;
        }
        else if (strcmp(command_line_words[i], ">>") == 0)
        {
            if (++output_count > 1 || redirection_type == 1)
            {
                fprintf(stderr, "Error! Can't have two >'s or >>'s!\n");
                return -1;
            }
            redirection_type = 2;
            output_fd = open(command_line_words[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (output_fd == -1)
            {
                perror("open");
                return -1;
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            // Shift the remaining arguments
            for (size_t j = i; j + 2 < *num_args; ++j)
            {
                command_line_words[j] = command_line_words[j + 2];
            }
            *num_args -= 2;
            i--;
        }
    }

    command_line_words[*num_args] = NULL; // Null-terminate the cleaned array

    return 0;
}

void execute_command(char **command_line_words, size_t num_args)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Handle I/O redirection
        if (handle_redirection(command_line_words, &num_args) != 0)
        {
            exit(EXIT_FAILURE);
        }

        execvp(command_line_words[0], command_line_words);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main()
{
    size_t num_args;

    // get the next command
    char **command_line_words = get_next_command(&num_args);
    while (command_line_words != NULL)
    {
        if (num_args > 0)
        {
            if (strcmp(command_line_words[0], "exit") == 0)
            {
                free_command(command_line_words);
                break;
            }
            execute_command(command_line_words, num_args);
        }
        // free the memory for this command
        free_command(command_line_words);

        // get the next command
        command_line_words = get_next_command(&num_args);
    }

    return 0;
}
