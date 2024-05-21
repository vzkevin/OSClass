#include <iostream>
#include <string>
#include <cstdlib>  // For std::exit
#include <sstream>  // For std::stringstream
#include <cmath>    // For std::fmod

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Insufficient arguments\n";
        std::exit(1);
    }

    std::string op = argv[1];
    double p;
    try {
        p = std::stod(argv[2]);
    } catch (...) {
        std::cerr << "Invalid number: " << argv[2] << '\n';
        std::exit(1);
    }

    double q;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        ss >> q;
        
        if (ss.fail()) {
            std::cerr << "Invalid input\n";
            continue;
        }
        
        if (op == "+") {
            std::cout << q + p << '\n';
        } else if (op == "-") {
            std::cout << q - p << '\n';
        } else if (op == "x") {
            std::cout << q * p << '\n';
        } else if (op == "/") {
            if (p == 0) {
                std::cerr << "Division by zero\n";
                continue;
            }
            std::cout << q / p << '\n';
        } else if (op == "%") {
            std::cout << std::fmod(q, p) << '\n';
        } else if (op == "==") {
            if (q == p) {
                std::cout << q << '\n';
            }
        } else if (op == "!=") {
            if (q != p) {
                std::cout << q << '\n';
            }
        } else if (op == "le") {
            if (q <= p) {
                std::cout << q << '\n';
            }
        } else if (op == "lt") {
            if (q < p) {
                std::cout << q << '\n';
            }
        } else if (op == "ge") {
            if (q >= p) {
                std::cout << q << '\n';
            }
        } else if (op == "gt") {
            if (q > p) {
                std::cout << q << '\n';
            }
        } else {
            std::cerr << "Unknown operator: " << op << '\n';
            std::exit(1);
        }
    }

    return 0;
}
