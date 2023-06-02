int nSteps = 0;
int emptyPos[2];
int board[10]; 

inline CanMove(i) {
  (board[i] != 0 && board[i + 1] != 0)
}

inline Move(i, j) {
  atomic {
    printf("Step %d: move (%d, %d) to (%d, %d)\n", nSteps + 1, i, i + 1, emptyPos[j], emptyPos[j] + 1);
    board[emptyPos[j]] = board[i];
    board[emptyPos[j] + 1] = board[i + 1];
    board[i] = 0;
    board[i + 1] = 0;
    emptyPos[j] = i;
    nSteps = nSteps + 1;
  }
}

inline Step(i, j) {
  CanMove(i) -> Move(i, j)
}

proctype main() {
  do
  :: Step(0, 0)
  :: Step(0, 1)
  :: Step(1, 0)
  :: Step(1, 1)
  :: Step(2, 0)
  :: Step(2, 1)
  :: Step(3, 0)
  :: Step(3, 1)
  :: Step(4, 0)
  :: Step(4, 1)
  :: Step(5, 0)
  :: Step(5, 1)
  :: Step(6, 0)
  :: Step(6, 1)
  :: Step(7, 0)
  :: Step(7, 1)
  :: Step(8, 0)
  :: Step(8, 1)
  od
}

init {
  board[0] = 0
  board[1] = 0
  board[2] = 0
  board[3] = 0
  board[4] = 1
  board[5] = 2
  board[6] = 1
  board[7] = 2
  board[8] = 1
  board[9] = 2

  emptyPos[0] = 0
  emptyPos[1] = 2
  
  run main();
}

ltl GoalNeverReached {
  !<>(board[0] == 2 && board[1] == 2 && board[2] == 2 && board[3] == 1 && board[4] == 1 && board[5] == 1 && nSteps == 3)
}

