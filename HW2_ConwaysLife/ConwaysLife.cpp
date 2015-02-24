#include <iostream>
#include <cstdio>
using namespace std;

enum tstate {_dead, _alive};

namespace simulator{

int iteration = 0, term;

struct tcell{
	int X, Y;
	tstate state[2];
	tcell * neighbour[4];
	tcell(tstate init_state = _dead){state = init_state;}
}

struct holder{
	tcell * corner[2][2];
}

void addStripe(){
	
}

void garbageCreator(int w, int h){
	
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
			if (!y || !x || y == h-1 || x == w-1) 
				corner[(bool)x][(bool)y] = new tcell()
}

void init(int w, int h, int alive, int it_requested){
	
	term = it_requested;
}

}
