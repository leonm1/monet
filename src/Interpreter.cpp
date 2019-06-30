//
// Created by hunter on 6/29/19.
//

#include "Interpreter.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>

Interpreter::Interpreter(std::string filename) {
  std::ifstream infile;
  infile.open(filename);
  std::string line;
  while (std::getline(infile, line)) {
    code.push_back(line);
  }
  interpret();
}

void Interpreter::interpret() {
  std::for_each(code.begin(), code.end(),
                [&](std::string line) -> void { eval(line); });
}

std::string Interpreter::eval(std::string value) {
  std::vector<std::string> words = split(value);
  if (words.size() == 0) {
    return "";
  } else if (words[0].length() > 1 && words[0].substr(0, 2) == "//") {
    // do nothing, this is a comment
    return "";
  } else if (words[0] == "quit") {
    quit(words);
  } else if (words[0] == "print") {
    print(words);
  } else if (words[0] == "println") {
    println(words);
  } else if (words[0] == "string") {
    declarestring(words);
  } else if (words[0] == "num") {
    declarenum(words);
  } else if (words[0] == "boolean") {
    declareboolean(words);
  } else if (words[0] == "read") {
    read(words);
  } else if (words[0] == "printall") {
    printcode();
  } else if (words[0] == "add") {
    return normalizenumber(add(words));
  } else if (words[0] == "sub") {
    return normalizenumber(sub(words));
  } else if (words[0] == "mul") {
    return normalizenumber(mul(words));
  } else if (words[0] == "div") {
    return normalizenumber(div(words));
  } else {
    throw std::logic_error("Function \"" + words[0] + "\" does not exist");
  }
  return "";
}

void Interpreter::printcode() {
  std::copy(code.begin(), code.end(),
            std::ostream_iterator<std::string>(std::cout, "\n"));
}

std::vector<std::string> Interpreter::split(std::string str, char delim) {
  std::vector<std::string> returnval;
  std::string temp = "";
  bool instr = false;
  bool inparens = false;
  for (uint32_t i = 0; i < str.length(); ++i) {
    if (str[i] == '(' && !instr) {
      inparens = true;
    }
    if (str[i] == ')' && !instr) {
      inparens = false;
    }
    if (str[i] == '"') {
      instr = !instr;
    }
    if (str[i] == delim && temp != "" && !instr && !inparens) {
      returnval.push_back(temp);
      temp = "";
    } else if (inparens) {
      temp += str[i];
    } else if (instr) {
      temp += str[i];
    } else {
      temp += tolower(str[i]);
    }
  }
  if (temp != "") {
    returnval.push_back(temp);
  }
  return returnval;
}

bool Interpreter::isString(std::string val) {
  return (val.length() >= 2 && val[0] == '"' && val[val.size() - 1] == '"');
}

int Interpreter::strtoint(std::string num) {
  std::stringstream ss(num);
  int val;
  ss >> val;
  return val;
}

double Interpreter::strtonum(std::string num) {
  std::stringstream ss(num);
  double val;
  ss >> val;
  return val;
}

std::string Interpreter::removequotes(std::string original) {
  if (isString(original)) {
    return original.substr(1, original.length() - 2);
  } else {
    return original;
  }
}

bool Interpreter::isParens(std::string statement) {
  return (statement.length() >= 2 && statement[0] == '(' &&
          statement[statement.size() - 1] == ')');
}

std::string Interpreter::removeparens(std::string original) {
  if (isParens(original)) {
    return original.substr(1, original.length() - 2);
  } else {
    return original;
  }
}

bool Interpreter::isNumber(std::string value) {
  if (value.length() == 0) {
    return false;
  }
  if (value[0] == '-' || isdigit(value[0])) {
    for (uint32_t x = 0; x < value.length(); x++) {
      if (!isdigit(value[x])) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

std::string Interpreter::normalizenumber(double x) {
  if (fmod(x, 1) < .000001) {
    return std::to_string(((int)x));
  } else {
    return std::to_string(x);
  }
}

void Interpreter::print(std::vector<std::string> words) {
  if (words.size() < 2) {
    std::cout << std::endl;
  } else {
    for (uint32_t i = 1; i < words.size(); ++i) {
      if (isString(words[i])) {
        std::cout << removequotes(words[i]) << std::flush;
      } else if (isParens(words[i])) {
        std::cout << eval(removeparens(words[i])) << std::flush;
      } else if (words[i] == "~") {
        std::cout << std::endl;
      } else {
        std::cout << memory.get(words[i]) << std::flush;
      }
    }
  }
}

void Interpreter::println(std::vector<std::string> words) {
  if (words.size() < 2) {
    // do nothing
  } else {
    for (uint32_t i = 1; i < words.size(); ++i) {
      if (isString(words[i])) {
        std::cout << removequotes(words[i]) << std::flush;
      } else if (isParens(words[i])) {
        std::cout << eval(removeparens(words[i])) << std::flush;
      } else if (words[i] == "~") {
        std::cout << std::endl;
      } else {
        std::cout << memory.get(words[i]) << std::flush;
      }
    }
  }
  std::cout << std::endl;
}

void Interpreter::quit(std::vector<std::string> words) {
  if (words.size() == 1) {
    exit(EXIT_SUCCESS);
  } else {
    if (isParens(words[1])) {
      exit(strtoint(eval(words[1])));
    } else {
      exit(strtoint(words[1]));
    }
  }
}

void Interpreter::declarestring(std::vector<std::string> vals) {
  if (vals.size() != 3) {
    throw std::logic_error(
        "Wrong number of parameters for string initialization");
  }
  if (isParens(vals[2])) {
    memory.createstring(vals[1], eval(vals[2]));
  } else {
    memory.createstring(vals[1], removequotes(vals[2]));
  }
}

void Interpreter::declareboolean(std::vector<std::string> vals) {
  if (vals.size() != 3) {
    throw std::logic_error(
        "Wrong number of parameters for boolean initialization");
  }
  bool a;
  if (isParens(vals[2])) {
    std::string temp = eval(vals[2]);
    a = (temp == "true" || temp == "t" || temp == "1") ? true : false;
  } else {
    a = (vals[2] == "true" || vals[2] == "t" || vals[2] == "1") ? true : false;
  }
  memory.createboolean(vals[1], a);
}

void Interpreter::declarenum(std::vector<std::string> vals) {
  if (vals.size() != 3) {
    throw std::logic_error("Wrong number of parameters for num initialization");
  }
  if (isParens(vals[2])) {
    memory.createnum(vals[1], strtonum(eval(vals[2])));
  } else {
    memory.createnum(vals[1], strtonum(vals[2]));
  }
}

void Interpreter::read(std::vector<std::string> vals) {
  if (vals.size() != 2) {
    throw std::logic_error("Wrong number of parameters for reading");
  }
  std::string input;
  std::cin >> input;
  memory.createstring(vals[1], input);
}

double Interpreter::add(std::vector<std::string> vals) {
  if (vals.size() < 2) {
    throw std::logic_error("Too few inputs for add");
  }
  std::vector<double> parameters;
  std::transform(vals.begin() + 1, vals.end(), std::back_inserter(parameters),
                 [&](std::string in) -> double {
                   if (isParens(in)) {
                     return strtonum(eval(removeparens(in)));
                   } else {
                     if (!isNumber(in)) {
                       return memory.getnum(in);
                     } else {
                       return strtonum(in);
                     }
                   }
                 });
  return std::accumulate(parameters.begin(), parameters.end(), 0);
}

double Interpreter::sub(std::vector<std::string> vals) {
  if (vals.size() < 2) {
    throw std::logic_error("Too few inputs for sub");
  }
  std::vector<double> parameters;
  std::transform(vals.begin() + 1, vals.end(), std::back_inserter(parameters),
                 [&](std::string in) -> double {
                   if (isParens(in)) {
                     return strtonum(eval(removeparens(in)));
                   } else {
                     if (!isNumber(in)) {
                       return memory.getnum(in);
                     } else {
                       return strtonum(in);
                     }
                   }
                 });
  return std::accumulate(parameters.begin(), parameters.end(), 0,
                         std::minus<>{});
}

double Interpreter::mul(std::vector<std::string> vals) {
  if (vals.size() < 2) {
    throw std::logic_error("Too few inputs for mul");
  }
  std::vector<double> parameters;
  std::transform(vals.begin() + 1, vals.end(), std::back_inserter(parameters),
                 [&](std::string in) -> double {
                   if (isParens(in)) {
                     return strtonum(eval(removeparens(in)));
                   } else {
                     if (!isNumber(in)) {
                       return memory.getnum(in);
                     } else {
                       return strtonum(in);
                     }
                   }
                 });
  return std::accumulate(parameters.begin(), parameters.end(), 1,
                         std::multiplies<>{});
}

double Interpreter::div(std::vector<std::string> vals) {
  if (vals.size() < 2) {
    throw std::logic_error("Too few inputs for div");
  }
  std::vector<double> parameters;
  std::transform(vals.begin() + 1, vals.end(), std::back_inserter(parameters),
                 [&](std::string in) -> double {
                   if (isParens(in)) {
                     return strtonum(eval(removeparens(in)));
                   } else {
                     if (!isNumber(in)) {
                       return memory.getnum(in);
                     } else {
                       return strtonum(in);
                     }
                   }
                 });
  double curr = parameters[0];
  for (uint32_t i = 1; i < parameters.size(); ++i) {
    curr /= parameters[i];
  }
  return curr;
}