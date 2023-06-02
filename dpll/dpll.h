#include <vector>
#include <stack>

enum Value {
  F_TRUE,
  F_FALSE,
  F_UNKNOWN
};

class CNF {
  friend std::ostream& operator <<(std::ostream &out, const CNF &cnf);

public:
  CNF(std::string path);
  bool check_sat();

private:
  int length;
  int var_num;
  std::vector<std::vector<int>> clauses;

  void spread_unit(std::vector<Value> &values, std::stack<std::tuple<int, Value, std::vector<int>>> &branches);
  void cancel_branching(std::vector<Value> &values, std::stack<std::tuple<int, Value, std::vector<int>>> &branches);
};