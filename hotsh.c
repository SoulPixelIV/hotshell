#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main (int argc, char **argv)
{
    shLoop();

    return EXIT_SUCCESS;
}

//Main loop
void shLoop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    }
    while (status);
}

//Read line
#define SH_RL_BUFSIZE 1024
char sh_read_line(void)
{
    int bufsize = SH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "sh: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();

        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;
    }

    if (position >= bufsize)
    {
        bufsize += SH_RL_BUFSIZE;
        buffer = realloc(buffer, bufsize);
        if (!buffer)
        {
            fprintf(stderr, "sh: Allocation error!\n");
            exit(EXIT_FAILURE);
        }
    }
}

//Split line
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "sh: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens)
            {
                fprintf(stderr, "sh: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
