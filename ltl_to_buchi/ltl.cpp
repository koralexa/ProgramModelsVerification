/*
 * Copyright 2021 ISP RAS (http://www.ispras.ru)
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
  switch (formula.kind()) {
  case Formula::ATOM:
    return out << formula.prop();
  case Formula::NOT:
    return out << "!(" << formula.arg() << ")";
  case Formula::AND:
    return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
  case Formula::OR:
    return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
  case Formula::IMPL:
    return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
  case Formula::X:
    return out << "X(" << formula.arg() << ")";
  case Formula::G:
    return out << "G(" << formula.arg() << ")";
  case Formula::F:
    return out << "F(" << formula.arg() << ")";
  case Formula::U:
    return out << "(" << formula.lhs() << ") U (" << formula.rhs() << ")";
  case Formula::R:
    return out << "(" << formula.lhs() << ") R (" << formula.rhs() << ")";
  }

  return out;
}

const Formula& simplify(const Formula &formula) {
  switch (formula.kind()) {
    case Formula::ATOM: {
      return formula;
    }
    case Formula::NOT: {
      Formula arg = simplify(formula.arg());
      if (arg.kind() == Formula::ATOM && arg.prop() == TRUE) {
        return P(FALSE);
      } else if (arg.kind() == Formula::ATOM && arg.prop() == FALSE) {
        return P(TRUE);
      } else {
        return !(simplify(formula.arg()));
      }
    }
    case Formula::AND: {
      Formula left = simplify(formula.lhs());
      Formula right = simplify(formula.rhs());
      if ((left.kind() == Formula::ATOM && left.prop() == FALSE) || (left.kind() == Formula::ATOM && left.prop() == FALSE)) {
        return P(FALSE);
      } else {
        return (simplify(formula.lhs()) && simplify(formula.rhs()));
      }
    }
    case Formula::OR: {
      Formula left = simplify(formula.lhs());
      Formula right = simplify(formula.rhs());
      if ((left.kind() == Formula::ATOM && left.prop() == TRUE) || (left.kind() == Formula::ATOM && left.prop() == TRUE)) {
        return P(TRUE);
      } else {
        return (simplify(formula.lhs()) || simplify(formula.rhs()));
      }
    }
    case Formula::IMPL: {
      return simplify(!(formula.lhs()) || (formula.rhs()));
    }
    case Formula::X: {
      Formula arg = simplify(formula.arg());
      switch (arg.kind()) {
        case Formula::ATOM:
          if (arg.prop() == TRUE) {
            return P(TRUE);
          } else if (arg.prop() == FALSE) {
            return P(FALSE);
          } else {
            return X(simplify(formula.arg()));
          }
        case Formula::OR:
          return simplify(X(arg.lhs()) || X(arg.rhs()));
        case Formula::AND:
          return simplify(X(arg.lhs()) && X(arg.rhs()));
        case Formula::NOT: {
          return simplify(!(X(arg.arg())));
        case Formula::U:
          return U(simplify(X(arg.lhs())), simplify(X(arg.rhs())));
        default:
          return X(simplify(formula.arg()));
        }
      }
    }
    case Formula::G: {
      return !(U(P(TRUE), !(simplify(formula.arg()))));
    }
    case Formula::F: {
      return U(P(TRUE), simplify(formula.arg()));
    }
    case Formula::U: {
      return U(simplify(formula.lhs()), simplify(formula.rhs()));
    }
    case Formula::R: {
      Formula rhs = simplify(formula.rhs());
      return (rhs || U(rhs, simplify(formula.lhs())));
    }
  }
  return formula;
}

bool Formula::operator ==(const Formula &f) const {
  if (this->kind() != f.kind()) {
    return false;
  }

  switch (this->kind()) {
    case ATOM: {
      return this->prop() == f.prop();
    }
    case NOT:
    case X:
    case G:
    case F: {
      return this->arg() == f.arg();
    }
    case AND:
    case OR:
      return (((this->lhs() == f.lhs()) && (this->rhs() == f.rhs())) || ((this->lhs() == f.rhs()) && (this->rhs() == f.lhs())));
    case IMPL:
    case U:
    case R: {
      return (this->lhs() == f.lhs()) && (this->rhs() == f.rhs());
    }
  }

  return false;
}

std::map<std::string, bool> get_atoms(std::vector<Formula> closure) {
  std::map<std::string, bool> result;

  for (auto &c : closure) {
    if ((c.kind() == Formula::ATOM && c.prop() != TRUE && c.prop() != FALSE) || (c.kind() == Formula::X)) {
      result.insert({c.prop(), false});
    }
  }

  return result;
}

FormulaValue check_value(const Formula &formula, std::map<std::string, bool> &atoms) {
  switch (formula.kind()) {
    case Formula::ATOM: {
      if (formula.prop() == TRUE) {
        return F_TRUE;
      } else if (formula.prop() == FALSE) {
        return F_FALSE;
      }
      return atoms[formula.prop()] ? F_TRUE : F_FALSE;
    }
    case Formula::NOT: {
      switch (check_value(formula.arg(), atoms)) {
        case F_TRUE: {
          return F_FALSE;
        }
        case F_FALSE: {
          return F_TRUE;
        }
        case F_UNKNOWN: {
          return F_UNKNOWN;
        }
      }
    }
    case Formula::AND: {
      FormulaValue left = check_value(formula.lhs(), atoms);
      FormulaValue right = check_value(formula.rhs(), atoms);
      if (left == F_UNKNOWN || right == F_UNKNOWN) {
        return F_UNKNOWN;
      } else if (left == F_TRUE && right == F_TRUE) {
        return F_TRUE;
      } else {
        return F_FALSE;
      }
    }
    case Formula::OR: {
      FormulaValue left = check_value(formula.lhs(), atoms);
      FormulaValue right = check_value(formula.rhs(), atoms);
      if (left == F_TRUE || right == F_TRUE) {
        return F_TRUE;
      } else if (left == F_UNKNOWN || right == F_UNKNOWN) {
        return F_UNKNOWN;
      } else {
        return F_FALSE;
      }
    }
    case Formula::X: {
      return atoms[formula.prop()] ? F_TRUE : F_FALSE;
    }
    case Formula::U: {
      return F_UNKNOWN;
    }
    default: {
      return F_UNKNOWN;
    }
  }
}

FormulaValue check_value(const Formula &formula, std::vector<Formula> &true_formulas) {
  switch (formula.kind()) {
    case Formula::X:
    case Formula::ATOM: {
      if (std::find(true_formulas.begin(), true_formulas.end(), formula) != true_formulas.end()) {
        return F_TRUE;
      } else {
        return F_FALSE;
      }
    }
    case Formula::NOT: {
      if (std::find(true_formulas.begin(), true_formulas.end(), formula) != true_formulas.end()) {
        return F_TRUE;
      } else {
        FormulaValue arg_value = check_value(formula.arg(), true_formulas);
        return arg_value == F_TRUE ? F_FALSE : (arg_value == F_FALSE ? F_TRUE : F_UNKNOWN);
      }
    }
    case Formula::AND: {
      if (std::find(true_formulas.begin(), true_formulas.end(), formula) != true_formulas.end()) {
        return F_TRUE;
      } else {
        FormulaValue left = check_value(formula.lhs(), true_formulas);
        FormulaValue right = check_value(formula.rhs(), true_formulas);
        if (left == F_UNKNOWN || right == F_UNKNOWN) {
          return F_UNKNOWN;
        } else if (left == F_TRUE && right == F_TRUE) {
          return F_TRUE;
        } else {
          return F_FALSE;
        }
      }
    }
    case Formula::OR: {
      if (std::find(true_formulas.begin(), true_formulas.end(), formula) != true_formulas.end()) {
        return F_TRUE;
      } else {
        FormulaValue left = check_value(formula.lhs(), true_formulas);
        FormulaValue right = check_value(formula.rhs(), true_formulas);
        if (left == F_UNKNOWN || right == F_UNKNOWN) {
          return F_UNKNOWN;
        } else if (left == F_TRUE || right == F_TRUE) {
          return F_TRUE;
        } else {
          return F_FALSE;
        }
      }
    }
    case Formula::U: {
      if (std::find(true_formulas.begin(), true_formulas.end(), formula) != true_formulas.end()) {
        return F_TRUE;
      } else {
        return F_UNKNOWN;
      }
    }
    default: {
      return F_UNKNOWN;
    }
  }
}

} // namespace model::ltl
