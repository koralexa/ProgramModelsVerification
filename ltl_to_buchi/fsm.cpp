#include "fsm.h"

namespace model::fsm {

std::ostream& operator <<(std::ostream &out, const State &state) {
  return out << state.label();
}

std::ostream& operator <<(std::ostream &out, const Transition &transition) {
  out << transition.source();
  out << " --[";

  bool separator = false;
  for (auto i = transition.symbol().begin(); i != transition.symbol().end(); i++) {
    out << (separator ? ", " : "") << *i;
    separator = true;
  }

  out << "]--> ";
  out << transition.target();

  return out;
}

std::ostream& operator <<(std::ostream &out, const Automaton &automaton) {
  bool separator;
 
  out << "S0 = {";
  separator = false;
  for (const auto &state: automaton._initial_states) {
    out << (separator ? ", " : "") << state;
  }
  out << "}" << std::endl;

  for (const auto &entry: automaton._final_states) {
    out << "F" << entry.first << " = {";
    separator = false;
    for (const auto &state: entry.second) {
      out << (separator ? ", " : "") << state;
    }
    out << "}" << std::endl;
  }

  out << "T = {" << std::endl;
  separator = false;
  for (const auto &entry: automaton._transitions) {
    for (const auto &transition: entry.second) {
      out << (separator ? "\n" : "") << "  " << transition;
      separator = true;
    }
  }
  out << std::endl << "}";

  return out;
}

std::vector<Formula> get_closure_with_duplicates(const Formula &formula) {
  std::vector<Formula> closure;
  switch (formula.kind()) {
    case Formula::ATOM:
      closure.push_back(formula);
      return closure;
    case Formula::NOT: {
      return get_closure_with_duplicates(formula.lhs());
    }
    case Formula::AND:
    case Formula::OR:
    case Formula::U: {
      closure = get_closure_with_duplicates(formula.lhs());
      std::vector<Formula> right_closure = get_closure_with_duplicates(formula.rhs());
      for (auto &f : right_closure) {
        closure.push_back(f);
      }
      closure.push_back(formula);
      return closure;
    }
    case Formula::X: {
      closure = get_closure_with_duplicates(formula.lhs());
      closure.push_back(formula);
      return closure;
    }
    default:
      return closure;
  }
  return closure;
}

std::vector<Formula> get_closure(const Formula &formula) {
  std::vector<Formula> closure = get_closure_with_duplicates(formula);
  std::vector<Formula> result;

  for (const auto &f : closure) {
    if (std::find(result.begin(), result.end(), f) == result.end()) {
      result.push_back(f);
    }
  }

  return result;
}

std::map<std::string, std::vector<Formula>> get_states(std::map<std::string, bool> atoms, std::vector<Formula> &closure) {
  std::map<std::string, std::vector<Formula>> result;
  int state_num = 1;

  for (int row_num = 0; row_num < (1 << atoms.size()); row_num++) {
    int atom_num = atoms.size() - 1;
    for (auto it = atoms.begin(); it != atoms.end(); it++) {
      it->second = (bool)(1 & (row_num >> atom_num));
      atom_num--;
    }

    std::vector<Formula> true_formulas;
    for (const auto &f : closure) {
      if (check_value(f, atoms) == F_TRUE) {
        true_formulas.push_back(f);
      }
    }

    std::map<std::string, std::vector<Formula>> tmp_states;

    tmp_states.insert({"s" + std::to_string(state_num), true_formulas});

    state_num++;

    for (auto &f : closure) {
      std::map<std::string, std::vector<Formula>> new_states;

      for (auto &s : tmp_states) {
        if (f.kind() == Formula::U) {
          if (std::find(s.second.begin(), s.second.end(), f.rhs()) != s.second.end() || 
             (f.rhs().kind() == Formula::NOT && std::find(s.second.begin(), s.second.end(), f.rhs().arg()) == s.second.end())) {
            s.second.push_back(f);
          } else if ((std::find(s.second.begin(), s.second.end(), f.lhs()) != s.second.end()) || 
                     (f.lhs().kind() == Formula::NOT && std::find(s.second.begin(), s.second.end(), f.lhs().arg()) == s.second.end())) {
            std::string name = "s" + std::to_string(state_num);
            state_num++;
            new_states.insert({name, s.second});
            new_states[name].push_back(f);
            s.second.push_back(!f);
          } else {
            s.second.push_back(!f);
          }
        } 
        
        if (check_value(f, s.second) == F_TRUE && std::find(s.second.begin(), s.second.end(), f) == s.second.end()) {
          s.second.push_back(f);
        }
      }

      for (auto &new_s : new_states) {
        tmp_states.insert({new_s.first, new_s.second});
      }
    }

    for (const auto &tmp_s : tmp_states) {
      result.insert({tmp_s.first, tmp_s.second});
    }
  }

  return result;
}

bool check_X_restrictions(std::vector<Formula> &formulas_X, const std::vector<Formula> &s1, const std::vector<Formula> &s2) {
  for (const auto &f_X : formulas_X) {
    if ((std::find(s1.begin(), s1.end(), f_X) != s1.end()) != (std::find(s2.begin(), s2.end(), f_X.arg()) != s2.end())) {
      return false;
    }
  }
  return true;
}

bool check_U_restrictions(std::vector<Formula> &formulas_U, const std::vector<Formula> &s1, const std::vector<Formula> &s2) {
  for (const auto &f_U : formulas_U) {
    if ((std::find(s1.begin(), s1.end(), f_U) != s1.end()) != 
        ((std::find(s1.begin(), s1.end(), f_U.rhs()) != s1.end()) || 
        ((std::find(s1.begin(), s1.end(), f_U.lhs()) != s1.end()) && (std::find(s2.begin(), s2.end(), f_U) != s2.end())))) {
      return false;
    }
  }
  return true;
}

std::map<std::string, std::pair<std::set<std::string>, std::set<std::string>>> get_transitions (std::vector<Formula> &closure, std::map<std::string, std::vector<Formula>> &states) {
  std::map<std::string, std::pair<std::set<std::string>, std::set<std::string>>> result;

  std::vector<Formula> formulas_X;
  std::vector<Formula> formulas_U;
  for (const auto &f : closure) {
    if (f.kind() == Formula::U) {
      formulas_U.push_back(f);
    }
    if (f.kind() == Formula::X) {
      formulas_X.push_back(f);
    }
  }

  for (const auto &s1 : states) {
    std::set<std::string> trans_symbols;
    for (const auto &f: s1.second) {
      std::string prop = f.prop();
      if (f.kind() == Formula::ATOM && prop != TRUE && prop != FALSE)
        trans_symbols.insert(prop);
    }

    result.insert({s1.first, {trans_symbols, {}}});
    
    for (const auto &s2 : states) {
      if (check_X_restrictions(formulas_X, s1.second, s2.second) && check_U_restrictions(formulas_U, s1.second, s2.second)) {
        result[s1.first].second.insert(s2.first);
      }
    }
  }

  return result;
}

std::set<std::string> get_initial_states(const Formula &formula, std::map<std::string, std::vector<Formula>> &states) {
  std::set<std::string> result;

  for (const auto &s : states) {
    if (std::find(s.second.begin(), s.second.end(), formula) != s.second.end()) {
      result.insert(s.first);
    }
  }

  return result;
}

std::map<int, std::set<std::string>> get_final_states(const std::vector<Formula> &closure, const std::map<std::string, std::vector<Formula>> &states) {
  std::map<int, std::set<std::string>> result;
  int set_num = 0;

  for (const auto &f : closure) {
    if (f.kind() == Formula::U) {
      result.insert({set_num, {}});
      for (const auto &s : states) {
        if ((std::find(s.second.begin(), s.second.end(), f) == s.second.end()) || (std::find(s.second.begin(), s.second.end(), f.rhs()) != s.second.end())) {
          result[set_num].insert(s.first);
        }
      }
      set_num++;
    }
  }

  return result;
}

Automaton get_ltl_to_buchi_automaton(const Formula& formula) {
  const Formula &simplified_formula = simplify(formula);

  std::vector<Formula> closure = get_closure(simplified_formula);
  std::vector<Formula> closure_with_negs;
  for (auto &f : closure) {
    closure_with_negs.push_back(f);
    closure_with_negs.push_back(!f);
  }
  closure.clear();

  std::map<std::string, bool> atoms = get_atoms(closure_with_negs);

  std::map<std::string, std::vector<Formula>> states = get_states(atoms, closure_with_negs);

  std::map<std::string, std::pair<std::set<std::string>, std::set<std::string>>> transitions = get_transitions(closure_with_negs, states);

  std::set<std::string> initial_states = get_initial_states(simplified_formula, states);

  std::map<int, std::set<std::string>> final_states = get_final_states(closure_with_negs, states);

  Automaton result;
  for (const auto &s : states) {
    result.add_state(s.first);
  }
  for (const auto &t : transitions) {
    for (const auto &s2 : t.second.second) {
      result.add_trans(t.first, t.second.first, s2);
    }
  }
  for (const auto &s : initial_states) {
    result.set_initial(s);
  }
  for (const auto &f_set : final_states) {
    for (const auto &s : f_set.second) {
      result.set_final(s, f_set.first);
    }
  }

  return result;
}

}