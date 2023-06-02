// каналы из условия
chan a = [0] of {bit};
chan b = [0] of {bit};
chan c = [0] of {bit};
chan d = [0] of {bit};
chan e = [0] of {bit};
chan f = [0] of {bit};
chan g = [0] of {bit};
chan h = [0] of {bit};
chan i = [0] of {bit};
chan j = [0] of {bit};
chan k = [0] of {bit};
chan l = [0] of {bit};
chan buffer1 = [2] of {bit};
chan buffer2 = [3] of {bit};

// source(out) — время от времени порождает маркер на выходе;
// записываем 1 в канал
proctype source(chan out) {
	do
	::	out!1
	od
}

// sink(in) — время от времени поглощает маркер на входе;
// читаем и удаляем из канала
proctype sink(chan in) {
	do
	::	in?_
	od
}

// fork(in; out1, out2) — по одному входному маркеру генерирует два выходных;
proctype fork(chan in, out1, out2) {
	do
	::	in?_ -> atomic {
		out1!1
		out2!1
	}
	od
}

// join(in1, in2; out) — по двум входным маркерам генерирует один выходной;
proctype join(chan in1, in2, out) {
	do
	:: in1?_ -> in2?_ -> atomic {
		out!1
	}
	od
}

// switch(in; out1, out2) — перенаправляет входной маркер на один из выходов;
proctype switch(chan in, out1, out2) {
	do
	:: in?_ -> atomic {
		out1!1
	}
	:: in?_ -> atomic {
		out2!1
	}
	od
}

// merge(in1, in2; out) — перенаправляет один из входных маркеров на выход;
proctype merge(chan in1, in2, out) {
	do
	:: in1?_ -> atomic {
		out!1
	}
	:: in2?_ -> atomic {
		out!1
	}
	od
}

// queue[N](in; out) — добавляет в буфер емкости N маркер со входа, выдает из буфера
// маркер на выход.

// один перекладывает маркер со входа в буфер (канал заданной емкости)
proctype queue_in(chan in, buf) {
	do
	:: in?_ -> atomic {
		buf!1
	}
	od
}

// второй — из буфера на выход
proctype queue_out(chan out, buf) {
	do
	:: buf?_ -> atomic {
		out!1
	}
	od
}

active proctype watchdog() {
	do
	:: timeout -> assert(false)
	od
}

init {
	run sink(a);
	run sink(b);
	run switch(c, a, b);
	run join(d, e, c);
	run queue_in(f, buffer1);
	run queue_out(d, buffer1);
	run queue_in(g, buffer2);
	run queue_out(e, buffer2); 
	run merge(h, i, f);
	run merge(j, k, g); 
	run source(h); 
	run source(k); 
	run fork(l, i, j);
	run source(l);
}

