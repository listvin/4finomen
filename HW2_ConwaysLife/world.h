#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>

//~ namespace worldspace{
using namespace std;

#define REP(n) for (int i = 0; i < (int)n; ++i)
enum tstate {_dead = 0, _alive = 1};

enum tdir {_east = 0, _north = 1, _west = 2, _south = 3};
tdir inc(tdir dir, int increment = 1){ int tmp = (int)dir + increment; while (tmp < 0) tmp += 4; return (tdir)(tmp % 4); };
tdir dec(tdir dir, int decrement = 1){ return inc(dir, -decrement); };
tdir opp(tdir dir){ return inc(dir, 2); };

struct tcell{
	tstate state[2] = {_dead, _dead};
	tcell * neighbour[4] = {NULL, NULL, NULL, NULL};
	tcell(){}
};

#define FOR_ALL_CELLS(it) \
	for (tcell * row = corner[0]; row != NULL; row = row->neighbour[1])\
		for (tcell * it = row; it != NULL; it = it->neighbour[2])

class tworld{
private:
	int T = 0; //time counter
	tcell * corner[4]; //corner hooks for cells
	int bound[4] = {1, 1, 0, 0};
	int alive[2]; //count of living cells

	int length(tdir dir){ return bound[dir] + bound[opp(dir)];}
	bool parity(bool current = false){ return (T + !current) & 1;}

public:
	int born, dead; //for statistics only
	int alive_count(){ return alive[parity(true)];}
	
private:
	void expand(tdir dir){
		//1. init locals
		tcell * hook = corner[dir];
		tcell * cur = NULL;
		tcell * prev = NULL;
		
		//2. create new cells
		while (hook){
			cur = new tcell();

			hook->neighbour[dir] = cur;
			if (prev)
				prev->neighbour[inc(dir, 1)] = cur;
			cur->neighbour[inc(dir, 2)] = hook;
			cur->neighbour[inc(dir, 3)] = prev;
			
			prev = cur;
			hook = hook->neighbour[inc(dir)];
		}
		
		//3. move corners outwards
		for (int i = 0 ; i < 2; ++i)
			corner[inc(dir, i)] = corner[inc(dir, i)]->neighbour[dir];
		
		//4. increase size
		++bound[dir];
	}
	void expansion(){
		bool pp = parity(false); //number of previous (irrelevant) moment of time in tcell.state[]
		REP(4){
			tdir dir = (tdir)i; //direction of possible expansion
			for (tcell * it = corner[dir]; it != NULL; it = it->neighbour[inc(dir)])
				if (it->state[pp]){
					expand(dir);
					break;
				}
		}
	}
	
	void reduce(tdir dir){
		if (length(dir) == 1) return;

		//1. init locals
		tcell * prev = NULL;
		tcell * cur = corner[dir];

		//2. move corners inwards
		for (int i = 0 ; i < 2; ++i)
			corner[inc(dir, i)] = corner[inc(dir, i)]->neighbour[opp(dir)];
		
		//3. delete extra cells
		while (cur){
			cur->neighbour[opp(dir)]->neighbour[dir] = NULL;
			
			prev = cur;
			cur = cur->neighbour[inc(dir)];
			delete prev;
		}
		
		//4. decrease size
		--bound[dir];		
	}
	void reduction(){
		bool cp = parity(true); //number of current (relevant) moment of time in tcell.state[]
		REP(4){
			tdir dir = (tdir)i; //direction of possible expansion
			bool to_reduce = true;
			while (length(dir) > 1 && to_reduce){
				for (tcell * it = corner[dir]->neighbour[opp(dir)]; it != NULL; it = it->neighbour[inc(dir)])
				if (it->state[cp]){
					to_reduce = false;
					break;
				}
				if (to_reduce) reduce(dir);
			}
		}
	}

	tstate det_state(tcell * p){
		assert(p);
		int c = 0; //count of alive cells
		bool pp = parity(false); //number of previous (irrelevant) moment of time in tcell.state[]
		for (int i = 0; i < 4; ++i){
			tdir dir = (tdir)i;
			if (p->neighbour[dir]){
				c += p->neighbour[dir]->state[pp];
				if (p->neighbour[dir]->neighbour[inc(dir, 1)])
					c += p->neighbour[dir]->neighbour[inc(dir, 1)]->state[pp];
				if (p->neighbour[dir]->neighbour[inc(dir, 3)])
					c += p->neighbour[dir]->neighbour[inc(dir, 3)]->state[pp];
			}
		}
		return c == 2 ? p->state[pp] : tstate(c == 3);
	}

public:
	void step(){
		//1. step the timer
		++T;
		
		//2. expand if necessary
		expansion();
		
		//3. renew population
		int pp = parity(false);
		int cp = parity(true);
		FOR_ALL_CELLS(it)
			it->state[cp] = det_state(it);
		
		//4. get stats
		born = dead = 0;
		FOR_ALL_CELLS(it)
			if (bool(it->state[pp]) ^ bool(it->state[cp])){
				if (it->state[cp] == _alive)
					++born;
				else
					++dead;
			}
				
		
		//5. reduce if possible
		reduction();
	}

	tworld(){
		corner[0] = corner[1] = corner[2] = corner[3] = new tcell();
	}

private:
	tworld(int W, int H) : tworld(){
		assert(W >= 1 && H >= 1);
		while (bound[_east] <= W) expand(_east);
		while (bound[_north] <= H) expand(_north);
	}
	
public:
	int width(){ return length(_east);}
	int height(){ return length(_north);}
	
	int minX(){ return -bound[_west];}
	int minY(){ return -bound[_south];}
	
	tworld(int W, int H, int aliveCount) : tworld(W, H){
		//1. init locals
		int N = W*H;
		assert(N >= aliveCount);
		
		//2. generate numbers for alive cells
		vector <int> a(N);
		REP(N) a[i] = i;
		REP(N) swap(a[i], a[rand()%(i+1)]);
		sort(&a[0], &a[aliveCount]);
		
		//3. walk around the world
		int i = 0;
		FOR_ALL_CELLS(it){
			++i;
			if (i == a[alive[0]]) it->state[0] = _alive, ++alive[0];
		}
	}
	
	tworld(ifstream * inf){
		string s; getline(*inf, s);
		*this = tworld(s.size(), s.size());
		
		for (tcell * row = corner[2]; row != NULL; row = row->neighbour[_south]){
			int i = 0;
			for (tcell * it = row; it != NULL; it = it->neighbour[_east], ++i)
				it->state[0] = s[i] == '0' || s[i] == ' ' || s[i] == '.' ? _dead : _alive;
			getline(*inf, s);
		}
	}

private:
	tcell * shook = NULL; //in order to make scope() faster
	int sx = 0, sy = 0, sh = 24; //in order to make scope() faster
	
	void print(){
		int cp = parity(true);
		tcell * hook = shook;
		int lsh = 1;
		REP(sh-1) if (hook->neighbour[_north]) hook = hook->neighbour[_north], ++lsh;
		
		int y = 0;
		for (tcell * row = hook; row != NULL && y < lsh; row = row->neighbour[1], ++y){
			int x = 0;
			for (tcell * it = row; it != NULL && x < sh; it = it->neighbour[2], ++x)
				putchar(it->state[cp] ? '#' : '+');
			printf("\tline №%d\n", y);
		}
	}
	
public:
	bool scope(int dx, int dy){
		assert(dx * dy == 0);
		if (!shook)
			shook = corner[3];
			sx = minX(),
			sy = minY();
		if (dx)
			if (dx > 0)
				if (shook->neighbour[_east]) shook = shook->neighbour[_east];
				else return false;
			else
				if (shook->neighbour[_west]) shook = shook->neighbour[_west];
				else return false;
		else
			if (dy > 0)
				if (shook->neighbour[_north]) shook = shook->neighbour[_north];
				else return false;
			else
				if (shook->neighbour[_south]) shook = shook->neighbour[_south];
				else return false;
		print();
		return true;			
	}
};

#undef REP
#undef FOR_ALL_CELLS
//~ }
