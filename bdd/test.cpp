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

#include <cassert>
#include <iostream>

#include "bdd.h"
#include "formula.h"

using namespace model::bdd;
using namespace model::logic;

int main() {
  Bdd bdd;

  const Formula &formula1 = x(0) >>  x(1);
  std::cout << "Formulae: " << std::endl;
  std::cout << formula1 << std::endl;
  const Node& root1 = bdd.create(formula1);
  std::cout << "BDD: " << std::endl;
  std::cout << root1 << std::endl << std::endl;

  const Formula &formula2 = !x(1) >> !x(0);
  std::cout << "Formulae: " << std::endl;
  std::cout << formula2 << std::endl;
  const Node& root2 = bdd.create(formula2);
  std::cout << "BDD: " << std::endl;
  std::cout << root2 << std::endl << std::endl;

  const Formula &formula3 = formula1 == formula2;
  std::cout << "Formulae: " << std::endl;
  std::cout << formula3 << std::endl;
  const Node& root3 = bdd.create(formula3);
  std::cout << "BDD: " << std::endl;
  std::cout << root3 << std::endl << std::endl;

  const Formula &formula4 = T;
  std::cout << "Formulae: " << std::endl;
  std::cout << formula4 << std::endl;
  const Node& root4 = bdd.create(formula4);
  std::cout << "BDD: " << std::endl;
  std::cout << root4 << std::endl << std::endl;

  const Formula &formula5 = (x(1) && x(2)) || (x(3) && x(4)) || (x(5) && x(6));
  std::cout << "Formulae: " << std::endl;
  std::cout << formula5 << std::endl;
  const Node& root5 = bdd.create(formula5);
  std::cout << "BDD: " << std::endl;
  std::cout << root5 << std::endl << std::endl;

  const Formula &formula6 = (x(1) && x(4)) || (x(2) && x(5)) || (x(3) && x(5));
  std::cout << "Formulae: " << std::endl;
  std::cout << formula6 << std::endl;
  const Node& root6 = bdd.create(formula6);
  std::cout << "BDD: " << std::endl;
  std::cout << root6 << std::endl << std::endl;

  assert(&root1 == &root2);
  assert(&root3 == &root4);
  assert(&root5 != &root6);

  return 0;
}
