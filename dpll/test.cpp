#include <iostream>
#include <cassert>
#include <ctime>

#include "dpll.h"

int main() {
  clock_t start;
  clock_t end;
  double time;

  start = clock();
  CNF test1("tests/seminars/test1.cnf");
  assert(test1.check_sat() == true);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 1 passed, time: " << time << " sec" << std::endl;

  start = clock();
  CNF test2("tests/seminars/test2.cnf");
  assert(test2.check_sat() == true);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 2 passed, time: " << time << " sec" << std::endl;

  // takes quite a long time (44 sec on my device)
  start = clock();
  CNF hanoi("tests/hanoi/hanoi4.cnf");
  assert(hanoi.check_sat() == true);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 3 passed, time: " << time << " sec" << std::endl;

  start = clock();
  CNF phole6("tests/phole/hole6.cnf");
  assert(phole6.check_sat() == false);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 4 passed, time: " << time << " sec" << std::endl;

  start = clock();
  CNF phole7("tests/phole/hole7.cnf");
  assert(phole7.check_sat() == false);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 5 passed, time: " << time << " sec" << std::endl;

  start = clock();
  CNF phole8("tests/phole/hole8.cnf");
  assert(phole8.check_sat() == false);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 6 passed, time: " << time << " sec" << std::endl;

  start = clock();
  CNF phole9("tests/phole/hole9.cnf");
  assert(phole9.check_sat() == false);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 7 passed, time: " << time << " sec" << std::endl;

  // takes quite a long time (6 min on my device)
  start = clock();
  CNF phole10("tests/phole/hole10.cnf");
  assert(phole10.check_sat() == false);
  end = clock();
  time = double(end - start) / CLOCKS_PER_SEC;
  std::cout << "Test 8 passed, time: " << time << " sec" << std::endl;
  
  return 0;
}
