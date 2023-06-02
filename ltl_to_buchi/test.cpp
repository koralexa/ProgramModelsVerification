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
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;

int main() {
  const Formula &formula = F(P("p") >> X(!(P("q"))));

  std::cout << "Formula: " << std::endl;
  std::cout << formula << std::endl << std::endl;

  std::cout << "Automaton: " << std::endl;
  std::cout << get_ltl_to_buchi_automaton(formula) << std::endl;

  return 0;
}

