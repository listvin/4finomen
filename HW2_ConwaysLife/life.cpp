#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <cmath>
#include "world.h"
using namespace std;

#define REP(n) for (int i = 0; i < (int)n; ++i)

int s2i(string s){
	stringstream ss;
	ss << " " << s << " ";
	int temp; 
	ss >> temp;
	return temp;
}

int S, H, W, C;
tworld world;
string F;

int main(int argc, char * arg[]){
	queue <string> q; REP(argc-1) q.push(arg[i+1]);

	#define GETn(pname, vname)\
		if (q.front() != pname) throw exception(); q.pop(),\
		vname = s2i(q.front()), q.pop();
	#define GETs(pname, vname)\
		if (q.front() != pname) throw exception(); q.pop(),\
		vname = q.front(), q.pop();
	
	try{
		switch(q.size()){
			case 8:
				GETn("--steps", S);
				GETn("-w", W);
				GETn("-h", H);
				GETn("-c", C);
				if (W > 0 && H > 0 && C <= H*W)
					world = tworld(W, H, C);
				else{
					cerr << "Incorrect size.\n";
					throw exception();
				}
			break;
			case 4:
				GETn("--steps", S);
				GETs("--file", F);
				{
					ifstream inf(F);
					if (inf.is_open())
						world = tworld(&inf);
					else{
						cerr << "File not found.\n";
						throw exception();
					}
				}
			break;
			case 1: default: // --help, lol
				printf("Usage:\n");
				printf("\tlife --steps <S> -w <W> -h <H> -c <C>\n");
				printf("\tlife --steps <S> --file <F>\n");
				printf("Where:\n");
				printf("\tS is count of steps to evaluate\n");
				printf("\tH and W are intitial height and width correspondingly\n");
				printf("\tC count of random alive cells to gen\n");
				printf("\tF is path to file with initial placing\n");
			return 0;
		}
	}
	catch(exception){
		printf("Incorrect paramline, try running \"life --help\".\n");
		return 0;
	}
	
	int pop0 = world.alive_count();
	int digits = (int)log10((double)S) + 1;
	int cnst_step = 0;
	REP(S){
		world.step();

		static int prv_pop = pop0;
		if (prv_pop == world.alive_count()) cnst_step = i+1; prv_pop = world.alive_count();

		static double sratio = 0;
		sratio += (double)world.born/world.dead;
		
		printf("DAY %*d. %10d are alive. <ratio> == %05lf", digits, i+1, world.alive_count(), sratio / (i%10+1));
		if ((i+1)%10) printf("\r"); else printf("\n"), sratio = 0;
	}
	if ((S-1)%10) cout << endl; else cout.flush();
	cout << "\nAll in all population " << (pop0 < world.alive_count() ? "insreased" : "decreased") << " by " << abs(world.alive_count()-pop0) << " cells." << endl;
	if (cnst_step) cout << "Population stayed constant last time after day " << cnst_step << endl;
	else cout << "Population have been changing for all evaluation time." << endl;
	return 0;
}

