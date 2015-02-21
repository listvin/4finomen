#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <stack>
#include <cstdlib>
#include <cassert>
#include <cstdlib>
#include <sstream>
using namespace std;

//~ #define MSVCPP
#define fname "arithmetic.py"
typedef long long int tdata;

tdata str2lli(string str_number){
	tdata int_number;
	stringstream ss;
	ss << " " + str_number + " ";
	ss >> int_number;
	return int_number;
}

bool debug_alert_sent = false;
enum exceptioncodes {excOK, excUknChar, excOaB0, excB1aB0, excOaO, excB1aO, excDivByZero, excModByZero, /*fixable to the right*/ excB0aD, excDaB1, excB0aB1};
string exceptionmessages[11] = {
	"Evaluated successfuly.",
	"Unknown char detected.",
	"Non-unary operator detected after opening braket.",
	"Empty brackets is not okay for this arithmetic.",
	"Two operators - one right by one.",
	"Bracket closes right after the operator, it's not OK.",
	"Division by zero somewhere...",
	"Calculating a number modulo zero somewhere...",
	"Have you forgotten * sign before the bracket after the number?",
	"Have you forgotten * sign before the number after the bracket?",
	"Have you forgotten * sign between two brackets?"
};
///B0 - opening bracket, B1 - closing, O - operator, D - digit. E - exception, when smth goes wrong inside the expression. T - terminal state, calculation goes off here
enum sets {setB0, setD, setO, setB1, setE};
enum states {stateB0, stateD, stateO, stateB1, stateE, stateT};
enum tokentypes {typeNUM = 0, typeMUL = '*', typeDIV = '/', typeMOD = '%', typeADD = '+', typeSUB = '-'};
struct ttoken{ ///Only two fields: If operator stored, type is not typeNUM, value is undefined. Otherwise value represents stored number.
	tokentypes type;
	tdata value;
	ttoken(tdata _value = 0){type = typeNUM, value = _value;} ///Empty constructor & number constructor.
	ttoken(tokentypes _type){value = -612, type = _type;} ///Operator constructor.
	ttoken(string number){type = typeNUM; value = str2lli(number);} ///Operator constructor.
};

//typicall construction for passage between automaton states
#define PASSAGE(action, new_state) {action; cur_state = (new_state);} break;
//consruction for passage to stateE
#define EMTHROW(excCode) {code = excCode; position = i; cur_state = stateE;} break;

namespace automaton{
	string s; int i;
	exceptioncodes code; int position;
	
	tdata evaluate(tdata a, tdata b, tokentypes t){
		switch(t){
			case typeMUL: return a*b;
			case typeDIV: if (b == 0) code = excDivByZero; else return a/b;
			case typeMOD: if (b == 0) code = excModByZero; else return a%b;
			case typeADD: return a+b;
			case typeSUB: return a-b;
			default: assert(!"Some strange flow, invalid call of \"evaluate(..)\".\n");
		}
		assert(!"Some strange flow, potential determination leak in \"evaluate(..)\".\n"); return 0;
	}
	
	sets classifyChar(char ch){
		if ((ch) == '(' || (ch) == '[') return setB0; else
		if ('0' <= (ch) && (ch) <= '9') return setD; else
		if ((ch) == '+' || (ch) == '-' || (ch) == '*' || (ch) == '/' || (ch) == '%') return setO; else
		if ((ch) == ')' || (ch) == ']') return setB1; else
		return setE;
	}

	tdata instance(){
		tdata result = 0;
		states cur_state = stateB0;
		stack <ttoken> st0, st1;
		string temp = "";
		while (i < (int)s.size()){
			switch(cur_state){
				case stateB0:
					switch(classifyChar(s[i])){	
						case setB0:	PASSAGE(++i; st0.push(ttoken(instance())), stateB1);
						case setD:	PASSAGE(temp = (string)("") + s[i], stateD);
						case setO:	///in order to support unary minus
							switch(s[i]){
								case typeSUB:	PASSAGE(st0.push(ttoken((tokentypes)s[i])), stateO);
								default: 		EMTHROW(excOaB0);
							}
						break;
						case setB1:	EMTHROW(excB1aB0);
						case setE:	EMTHROW(excUknChar);
					}
				break;
				case stateD:
					switch(classifyChar(s[i])){
						case setB0:	EMTHROW(excB0aD);
						case setD:	PASSAGE(temp = temp + s[i], stateD);
						case setO:	PASSAGE(st0.push(ttoken(temp)); st0.push(ttoken((tokentypes)s[i])), stateO);
						case setB1:	PASSAGE(st0.push(ttoken(temp)), stateT);
						case setE:	EMTHROW(excUknChar);
					}
				break;
				case stateO:
					switch(classifyChar(s[i])){
						case setB0:	PASSAGE(++i; st0.push(ttoken(instance())), stateB1);
						case setD:	PASSAGE(temp = (string)("") + s[i], stateD);
						case setO:	EMTHROW(excOaO);
						case setB1:	EMTHROW(excB1aO);
						case setE:	EMTHROW(excUknChar);
					}
				break;
				case stateB1:
					switch(classifyChar(s[i])){
						case setB0:	EMTHROW(excB0aB1);
						case setD:	PASSAGE(temp = (string)("") + s[i], stateD); //????
						case setO:	PASSAGE(st0.push(ttoken((tokentypes)s[i])), stateO);
						case setB1:	PASSAGE(, stateT);
						case setE:	EMTHROW(excUknChar);
					}
				break;
				case stateE:
					return 0;				
				break;
				case stateT:
					tokentypes t; tdata r;
					while (!st0.empty()) {st1.push(st0.top()); st0.pop();}
					while (!st1.empty() && code == excOK){ ///evaluating all 5-priority operators [*/%]
						switch(st1.top().type){
							case typeNUM: case typeADD: case typeSUB:
								st0.push(st1.top());
								st1.pop();
							break;
							case typeMUL: case typeDIV: case typeMOD:
								t = st1.top().type;
								st1.pop();
								assert(st1.top().type == typeNUM && st0.top().type == typeNUM);
								r = evaluate(st0.top().value, st1.top().value, t);
								st1.pop(); st0.pop();
								st0.push(r);
							break;
						}							
					}
					if (code) {cur_state = stateE; break;}
					while (!st0.empty()) {st1.push(st0.top()); st0.pop();}
					if (st1.top().type == typeNUM) result = st1.top().value, st1.pop();
					while (!st1.empty()){ ///evaluating all 6-priority operators (all left operators)[+-]
						t = st1.top().type;
						assert(t != typeNUM);
						st1.pop();
						assert(st1.top().type == typeNUM);
						result = evaluate(result, st1.top().value, t);
						st1.pop();
					}
					return result;
				break;	
			}
			if (cur_state != stateT && cur_state != stateE) ++i;
		}
		assert(!"Some strange flow, potential determination leak in \"instnce()\".\n");	return 0;
	}

	bool calc(string & s_expr, tdata & result){
		tdata exp_ans; bool have_test = false, erroneus = false;///string to store expected answer
		s = "", i = 1; ///initializing automaton's variables
		
		{///cleaning input from #-debug and whitespaces
			int j = -1;
			while (++j < (int)s_expr.size() && s_expr[j] != '#'){
				if (s_expr[j] != ' ' && s_expr[j] != '	') s = s + s_expr[j];
			}
			if (j+1 < (int)s_expr.size()){
				if (s_expr[j+1] == '#')
					erroneus = true;
				else{
					string temp = "";
					have_test = true;
					while (++j < (int)s_expr.size())
						if (s_expr[j] != ' ' && s_expr[j] != '	') temp = temp + s_expr[j];
					exp_ans = str2lli(temp);
				}
			}
			s = "(" + s + ")";
		}

		code = excOK; position = s.size()-1, s_expr = s; ///initializing exception control
		result = instance(); ///starting automaton

		{///interactive debugger trash=((
			if ((erroneus || have_test) && !debug_alert_sent) debug_alert_sent = cerr << "	Debugger message: hash-symbols found among input\ndata. Answers of this program for each expression-request\nwith hashes in the end will be finished with \":AC\" in case\nof success test and \":WA\" in case of fail. \":NT\" - means\nno test data.\n\n\n";
			if (debug_alert_sent) for (int i = 0; i < (int)s.size() + (code == excOK ? 5 + (int)ceil(log10(abs((double)result))) + (result < 0) : 2); ++i) printf(" ");
			if (have_test){
				if (result == exp_ans) printf(":AC\r");
				else printf(":WA\r");
			}
			if (erroneus){
				if (code == excOK) printf(":WA\r");
				else printf(":AC\r");
			}
			if (!erroneus && !have_test && debug_alert_sent) printf(":NT\r");
		}

		s_expr = s;
		return code == excOK;
	}
}
int main(){
	#ifdef fname
		cerr << "	Reading from file enabled: \"" fname "\".\n\n";
		freopen(fname, "r", stdin);
	#endif
	
	string s; tdata result = 0;
	while (getline(cin, s))
		if (automaton::calc(s, result))
			cerr << s << " = ", cout << result << endl, cerr << endl;
		else {
			cerr << s << "\n";		
			for (int i = 0; i < automaton::position; ++i) cerr << " "; cerr << "^ - " << exceptionmessages[automaton::code] << " (code " << automaton::code << ")\n\n";
		}
	
	#if MSVCPP
		freopen("CON","r",stdin), system("pause");
	#endif
	return 0;
}
