#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void execute_command(vector<vector<string>>& commands) {
    int num_commands = commands.size();
    int pipe_fds[num_commands - 1][2];  // Pipes array

    // Create pipes
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

            // Convert vector<string> to vector<char*> for execvp
            vector<char*> args;
            for (auto& arg : commands[i]) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            cerr << "Command execution failed: " << args[0] << '\n';
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
        if (waitpid(-1, &status, 0) == -1) {
            cerr << "Waitpid failed\n";
            exit(1);
        }

        // If any child process exits with a non-zero status, exit the parent with that status
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit(WEXITSTATUS(status));
        }
    }
}

int main() {
    cout << "Enter a pipeline command: ";
    string command_line;
    getline(cin, command_line);

    stringstream ss(command_line);
    string segment;
    vector<vector<string>> commands;

    // Split the command line by '|'
    while (getline(ss, segment, '|')) {
        stringstream command_stream(segment);
        string arg;
        vector<string> command_args;

        // Split the segment into command and arguments
        while (command_stream >> arg) {
            command_args.push_back(arg);
        }

        commands.push_back(command_args);
    }

    execute_command(commands);

    return 0;
}
