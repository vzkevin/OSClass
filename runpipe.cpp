#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void execute_command(vector<string>& commands) {
    int num_commands = commands.size();
    int pipe_fds[num_commands - 1][2];  

    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipe_fds[i]) == -1) {
            cerr << "Pipe creation failed\n";
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            cerr << "Fork failed\n";
            exit(1);
        } else if (pid == 0) {
            // Child process
    
            for (int j = 0; j < num_commands - 1; ++j) {
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            // Parse command and its arguments
            vector<char*> args;
            stringstream ss(commands[i]);
            string arg;
            while (ss >> arg) {
                args.push_back(strdup(arg.c_str()));
            }
            args.push_back(nullptr);

            // Execute command
            execvp(args[0], args.data());
            exit(1);
        }
    }

    // Close all pipe file descriptors in the parent process
    for (int i = 0; i < num_commands - 1; ++i) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    // Wait for all child processes to finish
    int status;
    for (int i = 0; i < num_commands; ++i) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit(WEXITSTATUS(status));
        }
    }
}

int main() {
    // print a prompt to read a string from the terminal
    cout << "Enter a pipeline command: ";
    string command_line;
    getline(cin, command_line);

    stringstream ss(command_line);
    string segment;
    vector<string> commands;

    // Split the command line by |
    while (getline(ss, segment, '|')) {
        commands.push_back(segment);
    }

    execute_command(commands);

    return 0;
}