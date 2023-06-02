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

#include "formula.h"

namespace model::logic {

const Formula Formula::F(Formula::FALSE);
const Formula Formula::T(Formula::TRUE);

const Formula F = Formula::F;
const Formula T = Formula::T;

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
  switch (formula.kind()) {
  case Formula::FALSE:
    return out << "false";
  case Formula::TRUE:
    return out << "true";
  case Formula::VAR:
    return out << "x" << formula.var();
  case Formula::NOT:
    return out << "!(" << formula.arg() << ")";
  case Formula::AND:
    return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
  case Formula::OR:
    return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
  case Formula::XOR:
    return out << "(" << formula.lhs() << ") != (" << formula.rhs() << ")";
  case Formula::IMPL:
    return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
  case Formula::EQ:
    return out << "(" << formula.lhs() << ") == (" << formula.rhs() << ")";
  }

  return out;
}

const Formula& apply(const Formula &formula, int var, bool value) {
  switch (formula.kind()) {
    case Formula::FALSE: {
      return F;
    }
    case Formula::TRUE: {
      return T;
    }
    case Formula::VAR: {
      if (formula.var() == var) {
        return value ? T : F;
      } else {
        return formula;
      }
    }
    case Formula::NOT: {
      Formula arg = apply(formula.arg(), var, value);
      if (arg.kind() == Formula::TRUE) {
        return F;
      } else if (arg.kind() == Formula::FALSE) {
        return T;
      }
      return !apply(formula.arg(), var, value);
    }
    case Formula::AND: {
      Formula left = apply(formula.lhs(), var, value);
      Formula right = apply(formula.rhs(), var, value);
      if ((left.kind() == Formula::FALSE) || (right.kind() == Formula::FALSE)) {
        return F;
      } else if (left.kind() == Formula::TRUE) {
        return apply(formula.rhs(), var, value);
      } else if (right.kind() == Formula::TRUE) {
        return apply(formula.lhs(), var, value);
      } else {
        return apply(formula.lhs(), var, value) && apply(formula.rhs(), var, value);
      }
    }
    case Formula::OR: {
      Formula left = apply(formula.lhs(), var, value);
      Formula right = apply(formula.rhs(), var, value);
      if ((left.kind() == Formula::TRUE) || (right.kind() == Formula::TRUE)) {
        return T;
      } else if (left.kind() == Formula::FALSE) {
        return apply(formula.rhs(), var, value);
      } else if (right.kind() == Formula::FALSE) {
        return apply(formula.lhs(), var, value);
      } else {
        return apply(formula.lhs(), var, value) || apply(formula.rhs(), var, value);
      }
    }
    case Formula::IMPL: {
      Formula left = apply(formula.lhs(), var, value);
      Formula right = apply(formula.rhs(), var, value);
      if (left.kind() == Formula::FALSE) {
        return T;
      } else if (right.kind() == Formula::TRUE) {
        return T;
      } else if ((left.kind() == Formula::TRUE) && (right.kind() == Formula::FALSE)) {
        return F;
      } else {
        return apply(formula.lhs(), var, value) >> apply(formula.rhs(), var, value);
      }
    }
    case Formula::XOR: {
      Formula left = apply(formula.lhs(), var, value);
      Formula right = apply(formula.rhs(), var, value);
      if (left.kind() == Formula::FALSE && right.kind() == Formula::FALSE) {
        return F;
      } else if (left.kind() == Formula::FALSE && right.kind() == Formula::TRUE) {
        return T;
      } else if (left.kind() == Formula::TRUE && right.kind() == Formula::FALSE) {
        return T;
      } else if (left.kind() == Formula::TRUE && right.kind() == Formula::TRUE) {
        return F;
      } else {
        return apply(formula.lhs(), var, value) != apply(formula.rhs(), var, value);
      }
    }
    case Formula::EQ: {
      Formula left = apply(formula.lhs(), var, value);
      Formula right = apply(formula.rhs(), var, value);
      if (left.kind() == Formula::FALSE && right.kind() == Formula::FALSE) {
        return T;
      } else if (left.kind() == Formula::FALSE && right.kind() == Formula::TRUE) {
        return F;
      } else if (left.kind() == Formula::TRUE && right.kind() == Formula::FALSE) {
        return F;
      } else if (left.kind() == Formula::TRUE && right.kind() == Formula::TRUE) {
        return T;
      } else {
        return apply(formula.lhs(), var, value) == apply(formula.rhs(), var, value);
      }
    }
  }
}

} // namespace model::logic
