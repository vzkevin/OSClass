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
    int pipe_fds[num_commands - 1][2];  // Pipes array

    // Create pipes
    for (int i = 0; i < num_commands - 1; ++i) {
        pipe(pipe_fds[i]);
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            cerr << "Fork failed\n";
            exit(1);
        } else if (pid == 0) {
            // Child process

            // If not the first command, set the input from the previous pipe
            if (i > 0) {
                if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) == -1) {
                    cerr << "dup2 input failed\n";
                    exit(1);
                }
            }

            // If not the last command, set the output to the next pipe
            if (i < num_commands - 1) {
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1) {
                    cerr << "dup2 output failed\n";
                    exit(1);
                }
            }

            // Close all pipe file descriptors in child processes
            for (int j = 0; j < num_commands - 1; ++j) {
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            vector<char*> args;
            stringstream ss(commands[i]);
            string arg;
            while (ss >> arg) {
                args.push_back(strdup(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            cerr << "Command execution failed: " << args[0] << '\n';
            exit(1);
        }
    }

    for (int i = 0; i < num_commands - 1; ++i) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    for (int i = 0; i < num_commands; ++i) {
        wait(nullptr);
    }
}

int main() {
    cout << "Enter a pipeline command: ";
    string command_line;
    getline(cin, command_line);

    stringstream ss(command_line);
    string segment;
    vector<string> commands;

    // Split the command line by '|'
    while (getline(ss, segment, '|')) {
        commands.push_back(segment);
    }

    execute_command(commands);

    return 0;
}
