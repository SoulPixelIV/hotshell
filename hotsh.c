#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

void sh_loop(void);
int sh_execute(char **args);
char *sh_read_line(void);
char **sh_split_line(char *line);
int sh_launch(char **args);
int sh_num_builtins(void);

//Other supported commands
int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);


char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &sh_cd,
  &sh_help,
  &sh_exit
};

int main(void)
{
    //Start Up Message
    printf("\033[37;1;5;41mSUPER HOT SHELL ACTIVATED\033[0m \n");
    sh_loop();
    return EXIT_SUCCESS;
}

//Main loop
void sh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        line = sh_read_line();
        //Save History
        if (line != NULL)
        {
            if (line[0] != 0)
            {
                add_history(line);
            }
        }
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    }
    while (status);
}

//Read line
char *sh_read_line(void)
{
    time_t rawtime;
    struct tm *info;
    char buffer[99];
    char buffer2[99];

    //Get current time
    time (&rawtime);
    info = localtime(&rawtime);

    while (1)
    {
        //Format time
        strftime(buffer, 99, "%X", info);
        //Get current Path
        getcwd(buffer2, 99);
        //Print time and path
        printf("\033[37;1;5;41m%s\n\033[31;1;5;47mHOT %s", buffer, buffer2);
        char *line = readline(" -> \033[0m");
        return line;
    }
}

//Split line in to multiple arguments
char **sh_split_line(char *line)
{
    char *token;
    char **tokens = malloc(64 * sizeof(char*));
    int position = 0;

    if (!tokens)
    {
        fprintf(stderr, "\033[37;1;5;41mSUPER ERROR\033[0m ");
        exit(EXIT_FAILURE);
    }
    //Split after a whitespace
    token = strtok(line, " ");

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        token = strtok(NULL, " ");
    }
    return tokens;
}

int sh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    //Clone shell and start other program
    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("\033[37;1;5;41mSUPER ERROR\033[0m");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("\033[37;1;5;41mSUPER ERROR\033[0m");
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int sh_num_builtins(void) {
  return sizeof(builtin_str) / sizeof(char *);
}

//Cd command
int sh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "\033[37;1;5;41m SUPER ERROR\033[0m");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("\033[37;1;5;41m SUPER ERROR\033[0m");
        }
    }
    return 1;
}

//Help command
int sh_help(char **args)
{
    int i;
    printf("\033[37;1;5;41mSuper Hot Shell (shsh) v.0.5\n");
    printf("By William Djalal\n");
    printf("GitHub: SoulPixelIV\n");
    printf("The following commands are built in:\033[0m\n");

    for (i = 0; i < sh_num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

//Exit command
int sh_exit(char **args)
{
    return 0;
}

int sh_execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1;
    }

    for (int i = 0; i < sh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return sh_launch(args);
}
