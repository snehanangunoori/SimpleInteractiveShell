// Including necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

// Defining macros for maximum commands and arguments
#define MAX_COMMANDS 100
#define MAX_ARGS 100

char *commands[MAX_COMMANDS]; // circular buffer to store history of commands
int commandCount = 0;         // number of commands currently in buffer
int startIndex = 0;           // index of the first command in the circular buffer
bool isPipe = false;          // flag to check if pipe is present in the command
bool history = false;         // flag to check if history command was entered
bool clearHistory = false;    // flag to check if history -c command was entered
char *args[MAX_ARGS];         // Global array to store arguments for execvp
char *line = NULL;            // Global variable to store the user input line

// Function to parse arguments from input line
void parseArgs(char *line)
{
    memset(args, 0, sizeof(args)); // Reset args array

    char *lineCopy = strdup(line); // Make a copy to avoid modifying the original input

    // Check if the command contains a pipe
    for (int i = 0; lineCopy[i] != '\0'; i++)
    {
        if (lineCopy[i] == '|')
        {
            isPipe = true;
            free(lineCopy);
            return;
        }
    }

    // Tokenize the input by spaces
    char *token = strtok(lineCopy, " ");
    int i = 0;

    while (token != NULL)
    {
        args[i++] = strdup(token);
        token = strtok(NULL, " ");
    }

    args[i] = NULL; // Null-terminate for execvp
    free(lineCopy);
}

// Add a command to the history buffer
void addCommand(char *command)
{
    char *commandCopy = strdup(command);
    if (commandCount == MAX_COMMANDS)
    {
        // Overwrite oldest command
        commands[startIndex++] = commandCopy;
    }
    else
    {
        commands[commandCount++] = commandCopy;
    }
}

// Clear the command history
void clearCommands()
{
    commandCount = 0;
    startIndex = 0;
}

// Show all commands in history
void showCommands()
{
    for (int i = 0; i < commandCount; i++)
    {
        int index = (startIndex + i) % MAX_COMMANDS;
        printf("%d %s\n", i, commands[index]);
    }
}

// Check if a string is a valid integer
int isInteger(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return 0;
        }
    }
    return 1;
}

// Function to handle piped commands
void pipeCommand(char *line)
{
    char *pipeCommands[100];       // Each command in the pipe
    char *pipeArgs[100][100];      // Arguments for each command
    char *lineCopy = strdup(line); // Copy to tokenize
    char *pipeToken = strtok(lineCopy, "|");
    int commandCount = 0;

    // Split input line by pipe
    while (pipeToken != NULL)
    {
        pipeCommands[commandCount] = pipeToken;
        pipeToken = strtok(NULL, "|");
        commandCount++;
    }

    // Tokenize each command into arguments
    for (int i = 0; i < commandCount; i++)
    {
        char *argToken = strtok(pipeCommands[i], " ");
        int argIndex = 0;
        while (argToken != NULL)
        {
            pipeArgs[i][argIndex++] = argToken;
            argToken = strtok(NULL, " ");
        }
        pipeArgs[i][argIndex] = NULL;
    }

    int prevfd = -1; // File descriptor for previous pipe

    for (int i = 0; i < commandCount; i++)
    {
        int fd[2];

        // Create pipe if not the last command
        if (i < commandCount - 1)
        {
            if (pipe(fd) == -1)
            {
                perror("Pipe failed");
            }
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            // Redirect input from previous pipe
            if (prevfd != -1)
            {
                dup2(prevfd, STDIN_FILENO);
                close(prevfd);
            }

            // Redirect output to next pipe
            if (i < commandCount - 1)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            // Execute command
            execvp(pipeArgs[i][0], pipeArgs[i]);
            perror("Error: execvp() failed!");
        }
        else if (pid < 0)
        {
            perror("Fork failed");
        }
        else
        {
            // Parent process closes unnecessary fds
            if (prevfd != -1)
            {
                close(prevfd);
            }
            if (i < commandCount - 1)
            {
                close(fd[1]);
                prevfd = fd[0];
            }
        }
    }

    // Wait for all children
    while (wait(NULL) > 0)
        ;
}

// Function to process user commands
void shell(char *args[])
{
    // Exit the shell
    if (strcmp(args[0], "exit") == 0)
    {
        free(line);
        line = NULL;
        exit(EXIT_SUCCESS);
    }
    // Change directory
    else if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            perror("Error: No directory specified");
        }
        else
        {
            int ch = chdir(args[1]);
            if (ch == -1)
            {
                perror("Error: Directory not found");
            }
        }
    }
    // Handle history commands
    else if (strcmp(args[0], "history") == 0)
    {
        if (args[1] == NULL)
        {
            history = true;
            addCommand(line);
            showCommands();
        }
        else if (strcmp(args[1], "-c") == 0)
        {
            clearHistory = true;
            clearCommands();
        }
        else if (isInteger(args[1]))
        {
            int offset = atoi(args[1]);
            if (offset < 0 || offset >= commandCount)
            {
                perror("Error: Invalid offset");
            }
            else
            {
                int index = (startIndex + offset) % MAX_COMMANDS;
                char *histLine = commands[index];
                parseArgs(histLine);
                shell(args); // Re-run the command
            }
        }
        else
        {
            fprintf(stderr, "Error: Invalid history command");
        }
    }
    // Execute external/system commands
    else
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
            perror("Error: execvp() failed!");
        }
        else if (pid > 0)
        {
            wait(NULL);
        }
        else
        {
            perror("Error: fork() failed!");
        }
    }
}

// Main loop
int main(int argc, char *argv[])
{
    size_t len = 0;
    ssize_t numChar;

    while (1)
    {
        printf("sish> ");
        fflush(stdout);

        numChar = getline(&line, &len, stdin);

        if (numChar == -1)
        {
            perror("getline error");
        }

        // Remove newline from end
        if (numChar > 0 && line[numChar - 1] == '\n')
        {
            line[numChar - 1] = '\0';
        }

        // Ignore empty input
        if (line[numChar - 1] == '\0' && numChar == 1)
        {
            free(line);
            line = NULL;
            continue;
        }

        parseArgs(line);

        if (isPipe)
        {
            pipeCommand(line);
        }
        else
        {
            shell(args);
        }

        // Only store in history if not already doing history stuff
        if (!history && !clearHistory)
        {
            addCommand(line);
        }

        // Reset for next command
        history = false;
        free(line);
        line = NULL;
        isPipe = false;
    }
    return 0;
}
