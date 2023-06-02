#include "bdd.h"

namespace model::bdd {

const Node Bdd::zero(-1, nullptr, nullptr);
const Node Bdd::one (-1, nullptr, nullptr);

int get_variable(const Formula &formula) {
  switch (formula.kind()) {
    case Formula::TRUE:
      return INT_MAX;
    case Formula::FALSE:
      return INT_MIN;
    case Formula::VAR:
      return formula.var();
    case Formula::NOT:
      return get_variable(formula.arg());
    case Formula::AND:
    case Formula::OR:
    case Formula::IMPL:
    case Formula::XOR:
    case Formula::EQ: {
      int left = get_variable(formula.lhs());
      int right = get_variable(formula.rhs());
      if (left == INT_MAX || left == INT_MIN) {
        return right;
      }
      if (right == INT_MAX || right == INT_MIN) {
        return left;
      }
      return left < right ? left : right;
    } 
  }
}

const Node& Bdd::compose(int var, const Node * low, const Node * high) {
  auto new_row = std::tuple<int, const Node *, const Node *>{var, low, high};
  auto existing_row = table.find(new_row);
  if (existing_row != table.end()){
    return existing_row->second;
  } else {
    table.insert({new_row, Node(var, low, high)});
    return table[new_row];
  }
}

const Node& Bdd::create(const Formula &formula) {
  if (formula.kind() == Formula::TRUE) {
    return one;
  }

  if (formula.kind() == Formula::FALSE) {
    return zero;
  }

  if (formula.kind() == Formula::VAR) {
    return compose(formula.var(), &zero, &one);
  }
  
  int var = get_variable(formula);

  if (var == INT_MAX) {
    return one;
  }

  if (var == INT_MIN) {
    return zero;
  }

  const Node &low = create(apply(formula, var, false));
  const Node &high = create(apply(formula, var, true));

  if (&low == &high) {
    return high;
  }

  return compose(var, &low, &high);
}

void print_tree(std::ostream &out, const std::string &prefix, const Node &row, bool high) {
  out << prefix << (high ? "------" : "- - - " );

  if (&row == &Bdd::zero) {
    out << "0" << std::endl;
    return;
  }

  if (&row == &Bdd::one) {
    out << "1" << std::endl;
    return;
  }

  out << "x" << row.var << std::endl;
  print_tree(out, prefix + "       ", *row.low, false);
  print_tree(out, prefix + "       ", *row.high, true);
}

std::ostream& operator <<(std::ostream &out, const Node &row) {
  print_tree(out, "", row, true);
  return out;
}

}