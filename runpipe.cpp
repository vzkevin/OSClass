#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void execute_command(vector<vector<string>>& commands) {
    // Number of commands we give
    int input_command = commands.size();

    // Array of pipes
    int pipes[input_command - 1][2];

    // pipes created
    for (int i = 0; i < input_command - 1; ++i) {
        pipe(pipes[i]);
    }

    // Go over each command
    for (int i = 0; i < input_command; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < input_command - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < input_command - 1; ++j) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            vector<char*> args;
            for (auto& arg : commands[i]) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            exit(1);
        }
    }

    // Close all pipes
    for (int i = 0; i < input_command - 1; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for child processes
    int status;
    for (int i = 0; i < input_command; ++i) {
        waitpid(-1, &status, 0);
    }
}

int main() {
    cout << "Enter a pipeline command: ";
    string commands;
    getline(cin, commands);

    vector<vector<string>> commands;
    stringstream ss(commands);
    string segment;
    vector<string> command_args;

    // Split the command line into segments with our |
    while (getline(ss, segment, '|')) {
        stringstream command_stream(segment);
        string arg;

        // Split each segment
        while (command_stream >> arg) {
            command_args.push_back(arg);
        }

        // Add
        commands.push_back(command_args);
        // Clear for the next command
        command_args.clear();
    }

    execute_command(commands);

    return 0;
}
