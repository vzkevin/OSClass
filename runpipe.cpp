void execute_command(vector<vector<string>>& commands) {
    int num_commands = commands.size();
    int pipe_fds[num_commands - 1][2];  // Pipes array

    // Create pipes
    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            // Child process

            // If not the first command, set the input from the previous pipe
            if (i > 0) {
                if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(1);
                }
            }

            // If not the last command, set the output to the next pipe
            if (i < num_commands - 1) {
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
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
            perror("execvp");
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
