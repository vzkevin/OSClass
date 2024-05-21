#include <iostream>
#include <string>
#include <limits>

using namespace std;

bool isArithmetic(const string& op) {
  return op == "+" || op == "-" || op == "x" || op == "/" || op == "%";
}

bool isRelational(const string& op) {
  return op == "==" || op == "!=" || op == "<=" || op == ">=" || op == "<" || op == ">";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " <operator> <value>" << endl;
    return 1;
  }

  string op(argv[1]);
  double p;
  stringstream(argv[2]) >> p;

  double q;
  while (cin >> q) {
    if (isArithmetic(op)) {
      switch (op[0]) {
        case '+': cout << q + p << endl; break;
        case '-': cout << q - p << endl; break;
        case 'x': cout << q * p << endl; break;
        case '/':
          if (p == 0) {
            cerr << "Error: Division by zero" << endl;
            return 1;
          }
          cout << q / p << endl;
          break;
        case '%': cout << fmod(q, p) << endl; break;
      }
    } else if (isRelational(op)) {
      bool result;
      switch (op[0]) {
        case '=': result = q == p; break;
        case '!': result = q != p; break;
        case '<': result = q < p; break;
        case '>': result = q > p; break;
        case 'L': result = q <= p; break;
        case 'G': result = q >= p; break;
      }
      if (result) {
        cout << q << endl;
      }
    } else {
      cerr << "Error: Unknown operator: " << op << endl;
      return 1;
    }
  }

  return 0;
}
