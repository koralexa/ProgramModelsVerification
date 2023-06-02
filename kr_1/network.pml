chan a = [0] of {bit};
chan b = [0] of {bit};
chan c = [0] of {bit};
chan d = [0] of {bit};
chan e = [0] of {bit};
chan f = [0] of {bit};
chan g = [0] of {bit};
chan h = [0] of {bit};

chan buf2 = [2] of {bit};
chan buf3 = [3] of {bit};

proctype source(chan out) {
  do
  :: out!1 -> 
  od
}

proctype sink(chan in1) {
  do
  :: in1?_ ->
  od
}

proctype fork(chan in1, out1, out2) {
  do
  :: in1?_ -> atomic{ out1!1; out2!1 }
  od
}

proctype join(chan in1, in2, out) {
  do
  :: in1?_ -> in2?_; atomic{ out!1 }
  od
}

proctype switch(chan in1, out1, out2) {
  do
  :: in1?_ -> atomic{ out1!1 };
  :: in1?_ -> atomic{ out2!1 };
  od
}

proctype merge(chan in1, in2, out) {
  do
  :: in1?_ -> atomic{ out!1 };
  :: in2?_ -> atomic{ out!1 };
  od
}

proctype queue1(chan in1, buf) {
  do
  :: in1?_ -> atomic{ buf!1 }
  od
}

proctype queue2(chan out, buf) {
  do
  :: buf?_ -> atomic{ out!1 }
  od
}

active proctype watchdog(){
	do
	:: timeout -> assert(false)
	od
}

init {
  atomic {
    run sink(a);
    run sink(b);
    run switch(c, a, e);
    run queue1(d, buf3);
    run queue2(c, buf3);
    run merge(e, f, d);
    run queue1(g, buf2);
    run queue2(b, buf2);
    run fork(h, f, g);
    run source(h);
  }
}