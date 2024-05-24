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
        pipe(pipe_fds[i]);
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process

            // If not the first command, set the input from the previous pipe
            if (i > 0) {
                dup2(pipe_fds[i - 1][0], STDIN_FILENO);
            }

            // If not the last command, set the output to the next pipe
            if (i < num_commands - 1) {
                dup2(pipe_fds[i][1], STDOUT_FILENO);
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
        waitpid(-1, &status, 0);
    }
}

int main() {
    while (true) {
        cout << "Enter a pipeline command (or 'exit' to quit): ";
        string command_line;
        getline(cin, command_line);

        if (command_line == "exit") {
            break;
        }

        vector<vector<string>> commands;
        stringstream ss(command_line);
        string segment;
        vector<string> command_args;

        // Split the command line into segments
        while (getline(ss, segment, '|')) {
            stringstream command_stream(segment);
            string arg;

            // Split each segment into command and arguments
            while (command_stream >> arg) {
                command_args.push_back(arg);
            }

            // Add this command and its arguments to the commands vector
            commands.push_back(command_args);
            // Clear the command_args vector for the next command
            command_args.clear();
        }

        execute_command(commands);
    }

    return 0;
}
