#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;

int main() {
  const Formula &formula = F(P("p") >> X(!(P("q"))));
    //  const Formula &formula = G(P("p") >> F(P("q")));
    // const Formula &formula = U(P("x"), U(P("y"), P("z")));
    //  const Formula& formula = G(P("p") >> ( !P("q") && ( U( X( !P("p") ), P("q")   ))));
    //  const Formula& formula = F(P("p") >> ( !P("q") && ( X(U(!P("p"), P("q")) ))));  /////////
  // const Formula& formula = G(F(X(P("p"))));
    //  const Formula& formula = F(P("p"));
    //  const Formula& formula = F(P("p")) ||  P("q");
    //  const Formula& formula = U( (P("p") || P("q")),(P("p") && P("q")) );
  // const Formula& formula = U(P("true"), P("p")) || P("q");

  std::cout << "Formula: " << std::endl;
  std::cout << formula << std::endl << std::endl;

  const Formula &simplified_formula = simplify(formula);
  std::cout << "Simplified formula: " << std::endl;
  std::cout << simplified_formula << std::endl << std::endl;

  std::vector<Formula> closure = get_closure(simplified_formula);
  std::vector<Formula> closure_with_negs;
  closure_with_negs.reserve(2 * closure.size());
  for (auto &f : closure) {
      closure_with_negs.push_back(f);
      closure_with_negs.push_back(!f);
  }
  closure.clear();
  std::cout << "Closure: " << std::endl;
  for (int i = 0; i < closure_with_negs.size(); i++) {
    // if (closure_with_negs[i].kind() != Formula::NOT)
      std::cout << closure_with_negs[i] << std::endl;
  }
  std::cout << std::endl;

  std::map<std::string, bool> atoms = get_atoms(closure_with_negs);
  std::cout << "Atoms: " << std::endl;
  for(const auto& elem : atoms) {
    std::cout << elem.first << " " << elem.second << std::endl;
  }
  std::cout << std::endl;

  std::map<std::string, std::vector<Formula>> states = get_states(atoms, closure_with_negs);
  std::cout << "States: " << std::endl;
  for(const auto& elem : states) {
    std::cout << elem.first << ": ";
    for(const auto& f : elem.second) {
      if (f.kind() != Formula::NOT) {
      std::cout << f << ", ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  std::map<std::string, std::pair<std::set<std::string>, std::set<std::string>>> transitions = get_transitions(closure_with_negs, states);
  std::cout << "Transitions: " << std::endl;
  for(const auto& elem : transitions) {
    std::cout << elem.first << std::endl;
    std::cout << "Symbols: " << std::endl;
    for (const auto &s : elem.second.first) {
      std::cout << s << " ";
    }
    std::cout << std::endl;
    std::cout << "States: " << std::endl;
    for (const auto &s : elem.second.second) {
      std::cout << s << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }
  std::cout << std::endl;

  std::set<std::string> initial_states = get_initial_states(simplified_formula, states);
  std::cout << "Initial states: " << std::endl;
  for(const auto& elem : initial_states) {
    std::cout << elem << std::endl;
  }
  std::cout << std::endl;

  std::map<int, std::set<std::string>> final_states = get_final_states(closure_with_negs, states);
  std::cout << "Final states: " << std::endl;
  for(const auto& elem : final_states) {
    std::cout << "Group " <<elem.first << std::endl;
    for(const auto& s : elem.second) {
      std::cout << s << std::endl;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Automaton: " << std::endl;
  std::cout << get_ltl_to_buchi_automaton(formula) << std::endl;

  // Automaton automaton;
  // automaton.add_state("s0");
  // automaton.add_state("s1");
  // automaton.add_state("s2");
  // automaton.set_initial("s0");
  // automaton.set_final("s1", 0);
  // automaton.set_final("s2", 1);
  // automaton.add_trans("s0", {"p"}, "s1");
  // automaton.add_trans("s0", {"q"}, "s2");
  // automaton.add_trans("s1", {}, "s0");
  // automaton.add_trans("s1", {"p", "q"}, "s2");
  // automaton.add_trans("s2", {}, "s2");

  // std::cout << "Automaton: " << std::endl;
  // std::cout << automaton << std::endl;

  return 0;
}