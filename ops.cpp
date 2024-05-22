// This program takes an operator and a number as arguments, then reads numbers from 
// standard input, performs the specified operation between each number and the first 
// argument number, and prints the results.

#include <iostream>
#include <string>
#include <cstdlib>  // For exit
#include <sstream>  // For stringstream
#include <cmath>    // For fmod

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Insufficient arguments\n";
        exit(1);
    }

    string op = argv[1];
    double p;
    try {
        p = stod(argv[2]);
    } catch (...) {
        cerr << "Invalid number: " << argv[2] << '\n';
        exit(1);
    }

    double q;
    string line;
    
    while (getline(cin, line)) {
        stringstream ss(line);
        ss >> q;
        
        if (ss.fail()) {
            cerr << "Invalid input\n";
            continue;
        }
        
        if (op == "+") {
            cout << q + p << '\n';
        } else if (op == "-") {
            cout << q - p << '\n';
        } else if (op == "x") {
            cout << q * p << '\n';
        } else if (op == "/") {
            if (p == 0) {
                cerr << "Division by zero\n";
                continue;
            }
            cout << q / p << '\n';
        } else if (op == "%") {
            cout << fmod(q, p) << '\n';
        } else if (op == "==") {
            if (q == p) {
                cout << q << '\n';
            }
        } else if (op == "!=") {
            if (q != p) {
                cout << q << '\n';
            }
        } else if (op == "le") {
            if (q <= p) {
                cout << q << '\n';
            }
        } else if (op == "lt") {
            if (q < p) {
                cout << q << '\n';
            }
        } else if (op == "ge") {
            if (q >= p) {
                cout << q << '\n';
            }
        } else if (op == "gt") {
            if (q > p) {
                cout << q << '\n';
            }
        } else {
            cerr << "Unknown operator: " << op << '\n';
            exit(1);
        }
    }

    return 0;
}
