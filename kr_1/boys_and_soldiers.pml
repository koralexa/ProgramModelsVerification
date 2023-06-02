bit boy1 = 0;
bit boy2 = 0;
bit soldier1 = 0;
bit soldier2 = 0;
bit soldier3 = 0;
bit boat = 0;

active proctype main() {
  do
  :: (boat == boy1) -> atomic { printf("boy1\n"); boat = 1 - boat; boy1 = 1 - boy1 }
  :: (boat == boy2) -> atomic { printf("boy2\n"); boat = 1 - boat; boy2 = 1 - boy2 }
  :: (boat == soldier1) -> atomic { printf("soldier1\n"); boat = 1 - boat; soldier1 = 1 - soldier1 }
  :: (boat == soldier2) -> atomic { printf("soldier2\n"); boat = 1 - boat; soldier2 = 1 - soldier2 }
  :: (boat == soldier3) -> atomic { printf("soldier3\n"); boat = 1 - boat; soldier3 = 1 - soldier3 }
  :: (boat == boy1 && boat == boy2) -> atomic { printf("boy1 and boy2\n"); boat = 1 - boat; boy1 = 1 - boy1; boy2 = 1 - boy2 }
  od
}

ltl GoalNeverReached {
  !(<>(boat && boy1 && boy2 && soldier1 && soldier2 && soldier3))
}