#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "dpll.h"

std::ostream & operator<<(std::ostream &out, const CNF &cnf) {
  out << "CNF length: " << cnf.length << std::endl;
  out << "CNF variables num: " << cnf.var_num << std::endl;
  out << "Clauses: " << std::endl;

  for (int i = 0; i < cnf.length; i++) {
    if (i != 0) {
      out << "&";
    } else {
      out << " ";
    }
    out << " (";

    for (int j = 0; j < cnf.clauses[i].size(); j++) {
      if (j != 0)
          out << " \\/ ";
      if (cnf.clauses[i][j] >= 0) {
        out << "x" << cnf.clauses[i][j];
      } else {
        out << "~x" << -cnf.clauses[i][j];
      }
    }

    out << ")" << std::endl;
  }

  return out;
}

CNF::CNF(std::string path) {
  std::ifstream s(path);
  std::string line;
  length = 0;
  var_num = 0;

  while (std::getline(s, line)) {
    if ((line != "") && (line != "0") && (line[0] != '%') && (line[0] != 'c')) {
      if (line[0] == 'p') {
        std::stringstream ss(line);
        std::string buf;
        ss >> buf >> buf >> var_num >> length;
      } else {
        std::stringstream ss(line);
        std::vector<int> c;
        int var;
        while ((ss >> var) && (var != 0)) {
          c.push_back(var);
        }
        clauses.push_back(c);
      }
    }
  }
}

Value get_value(int num, std::vector<Value> &values) {
  int idx = abs(num) - 1;
  if ((num > 0) || (values[idx] == Value::F_UNKNOWN)) {
    return values[idx];
  } else {
    return values[idx] == Value::F_TRUE ? Value::F_FALSE : Value::F_TRUE;
  }
}

void CNF::spread_unit(std::vector<Value> &values, std::stack<std::tuple<int, Value, std::vector<int>>> &branches) {
  for (auto &c : clauses) {
    int clause_len = 0;
    int var;
    for (auto it : c) {
      if (clause_len > 1) {
        break;
      } else if (get_value(it, values) == Value::F_UNKNOWN) {
        var = it;
        clause_len++;
      } else if (get_value(it, values) == Value::F_TRUE) {
        clause_len = 0;
        break;
      }
    }

    if (clause_len == 1) {
      values[abs(var) - 1] = var > 0 ? Value::F_TRUE : Value::F_FALSE;
      if (!branches.empty()) {
        std::get<2>(branches.top()).push_back(var > 0 ? var - 1 : var + 1);
      }
    }
  }
}

void CNF::cancel_branching(std::vector<Value> &values, std::stack<std::tuple<int, Value, std::vector<int>>> &branches) {
  for (auto &v : std::get<2>(branches.top())) {
    values[abs(v)] = Value::F_UNKNOWN;
  }
  std::get<2>(branches.top()).clear();

  if (std::get<1>(branches.top()) == Value::F_TRUE) {
    std::get<1>(branches.top()) = Value::F_FALSE;
    values[abs(std::get<0>(branches.top()))] = Value::F_FALSE;
  } else {
    values[abs(std::get<0>(branches.top()))] = Value::F_UNKNOWN;
    branches.pop();
    if (!branches.empty()) {
      cancel_branching(values, branches);
    }
  }
}

bool CNF::check_sat() {
  std::vector<Value> values = std::vector<Value>(var_num, Value::F_UNKNOWN);
  std::stack<std::tuple<int, Value, std::vector<int>>> branches;

  while (true) {
    spread_unit(values, branches);

    bool has_false_clause = false;
    for (const auto &c : clauses) {
      bool clause_false = true;
      for (auto it : c) {
        if (get_value(it, values) != Value::F_FALSE) {
          clause_false = false;
          break;
        }
      }
      if (clause_false) {
        has_false_clause = true;
        break;
      }
    }

    if (has_false_clause) {
      if (!branches.empty()) {
        cancel_branching(values, branches);
      }
      if (branches.empty()) {
        return false;
      }
    } else {
      bool all_true = true;
      for (const auto &c : clauses) {
        bool clause_true = false;
        for (auto it : c) {
          if (get_value(it, values) == Value::F_TRUE) {
            clause_true = true;
            break;
          }
        }
        if (!clause_true) {
          all_true = false;
          break;
        }
      }

      if (all_true) {
        return true;
      }

      int idx = 0;
      while ((idx < var_num) && (get_value(idx + 1, values) != Value::F_UNKNOWN)) {
        idx++;
      }
      branches.push(std::tuple<int, Value, std::vector<int>>(idx, Value::F_TRUE, {}));
      values[idx] = Value::F_TRUE;
    }
  }
}