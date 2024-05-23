#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

// Function to split a string by delimiter
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    string input;

    // Print prompt and read input
    cout << "Enter a pipeline command: ";
    getline(cin, input);

    // Parse the input into individual commands
    vector<string> commands = split(input, '|');
    int num_commands = commands.size();

    // Array of file descriptors for the pipe
    int pipe_fd[2];
    int prev_fd = -1; // File descriptor for the previous command

    for (int i = 0; i < num_commands; ++i) {
        pipe(pipe_fd); // Create a pipe

        pid_t pid = fork();
        if (pid == -1) {
            cerr << "Failed to fork" << endl;
            return 1;
        }

        if (pid == 0) {
            // Child process
            if (prev_fd != -1) {
                dup2(prev_fd, 0); // Redirect stdin to the previous pipe's read end
                close(prev_fd);
            }
            if (i < num_commands - 1) {
                dup2(pipe_fd[1], 1); // Redirect stdout to the current pipe's write end
                close(pipe_fd[1]);
            }
            close(pipe_fd[0]);

            // Parse the command and its arguments
            vector<string> cmd_args = split(commands[i], ' ');
            vector<char*> args;
            for (const string& arg : cmd_args) {
                args.push_back((char*)arg.c_str());
            }
            args.push_back(nullptr);

            // Execute the command
            execvp(args[0], args.data());
            cerr << "Failed to execute " << args[0] << endl;
            exit(1);
        } else {
            // Parent process
            waitpid(pid, nullptr, 0); // Wait for the child process to finish
            close(pipe_fd[1]); // Close the write end of the pipe
            if (prev_fd != -1) {
                close(prev_fd);
            }
            prev_fd = pipe_fd[0]; // Save the read end of the pipe for the next command
        }
    }

    return 0;
}
