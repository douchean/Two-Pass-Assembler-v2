#include "Parser.h"



void Parser::checkSymbols() { //izmeniti, omoguciti listu obaveznih konstanti
	konstante.clear();
	vector<string> tokens = tokenizuj(izraz);
	if (greska == true) return;
	//int tokensize = tokens.size();
		int position = 0; ////samo dva simbola sa minusom
		bool numanuma = false; //desila se konstanta
		for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {
			if ((*it) == "") continue;
			position++;
			if (!isNumber(*it)) {//prvi je simbol
				if (position > 2 || numanuma == true) {  // ovo mora biti konstanta
					Simbol* sim;
					if (tabelaSimbola->find(*it) == tabelaSimbola->end()) {
						sim = new Simbol();
						sim->setLocal();
						sim->setDef(false);
						sim->setSek(new Sekcija("UND"));
						sim->setRbr(tabelaSimbola->size() + 1);
						sim->setNaziv(*it);
						(*tabelaSimbola)[*it] = sim;
					}
					else sim = (*tabelaSimbola)[(*it)];
					konstante.push_back(sim);
				}
				else
					if (position == 2) {
						bool checkMinus = false;
						for (vector<char>::iterator it2 = operatori.begin(); !checkMinus; it2++) {
							if ((*it2) == '-') checkMinus = true;
							else if ((*it2) != '(' && (*it2) != ')') {
								/*cout << "Izmedju dve labele mora minus!  " << izraz << endl;
								getchar();
								exit(-1);
								return;*/
								Simbol* sim;
								if (tabelaSimbola->find(*it) == tabelaSimbola->end()) {
									sim = new Simbol();
									sim->setLocal();
									sim->setDef(false);
									sim->setSek(new Sekcija("UND"));
									sim->setRbr(tabelaSimbola->size() + 1);
									sim->setNaziv(*it);
									(*tabelaSimbola)[*it] = sim;
								}
								else sim = (*tabelaSimbola)[(*it)];
								konstante.push_back(sim);
								//bro
							}
						}
					}
				if (tabelaSimbola->find(*it) == tabelaSimbola->end()) {
					Simbol* sim = new Simbol();
					sim->setLocal();
					sim->setDef(false);
					sim->setSek(new Sekcija("UND"));
					sim->setRbr(tabelaSimbola->size() + 1);
					sim->setNaziv(*it);
					(*tabelaSimbola)[*it] = sim;
				} 
			}
			else
				numanuma = true;

		}
	
	
}

void Parser::checkSymbols(Sekcija* sekcija) { //zameniti u stringu izraza konstante njihovim vrednostima, potrebna lista postojecih konstanti
	this->sekcija = sekcija;
	vector<string> tokens = tokenizuj(izraz);

	Sekcija* sek1; Sekcija* sek2;
	Simbol* sim1; Simbol* sim2;
	bool global1 = false; bool prviUbacen = false;
	for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++) {

		for (vector<Simbol*>::iterator iter = konstante.begin();
			iter != konstante.end(); ++iter) {
			if ((*iter)->getNaziv() == (*it)) {
				//(*it) = (*iter)->getVrednost(); 
				string Result;
				ostringstream convert;
				convert << (*iter)->getVrednost();

				Result = convert.str();
				izraz = izraz.replace(izraz.find(*it), (*it).length(), Result); //check if good
				(*it) = Result;
			}
		}
		if ((*it) == "") continue;
		if (!isNumber(*it)) {//prvi je simbol
			if (tabelaSimbola->find(*it) == tabelaSimbola->end()) {
				greska = true;
				cout << "Ne postoji simbol!" << izraz;
				getchar();
				exit(1);
			}
			if(!prviUbacen){
				sim1 = (*tabelaSimbola)[*it];
				sek1 = sim1->getSek();
				global1 = sim1->isExtern();
				prviUbacen = true;
				if (sek1->getNaziv() != sekcija->getNaziv()) {
					trebaRelokacija = true;
					simbolZaRelokaciju = sim1;
				}

				int value = sim1->getVrednost();
				string Result;
				ostringstream convert;
				convert << value;
				Result = convert.str();
				string izraz2 = Result; //zameniti u stringu razliku labela vrednoscu njihove razlike
				izraz = izraz.substr(izraz.find(*it) + (*it).size());
				izraz2 = izraz2 + izraz;
				izraz = izraz2;
			}
			else { //postoje dva simbola u izrazu
				sim2 = (*tabelaSimbola)[*it];
				sek2 = sim2->getSek();
				if (global1 || sim2->isGlobal()) {
					cout << "Los izraz: simbol u izrazu je globalan!" << izraz << endl;
					getchar();
					exit(1);
				}
				if (sek1->getNaziv() != sek2->getNaziv()) {
					cout << "Labele u izrazu moraju biti iz iste sekcije!" << izraz << endl;
					getchar();
					exit(1);
				}
				else {
					trebaRelokacija = false;
				}
				int value = sim1->getVrednost() - sim2->getVrednost();
				trebaRelokacija = false;
				string Result;
				ostringstream convert;
				convert << value;
				Result = convert.str();
				string izraz2 = Result; //zameniti u stringu razliku labela vrednoscu njihove razlike
				izraz = izraz.substr(izraz.find(*it) + (*it).size());
				izraz2 = izraz2 + izraz;
				izraz = izraz2;
			}
		}

	}


}


bool Parser::isNumber(std::string s){
	if (s.find("0x") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("0123456789ABCDEFabcdef") == string::npos)
			return true;
		else return false;
	} else if (s.find("0b") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("01") == string::npos)
			return true;
		else return false;
	}
	if(s.find_first_not_of("0123456789") == string::npos)
		return true;
	return false;
}


/*
string Parser::relok(string iz) {
	if (isNumber(iz)) return "";
	//vector<int>CrniPopravi;
	string operand, operat;

	if (iz.find_first_of("R") != string::npos && iz.find_first_of("[") != string::npos && iz.find_first_of("]") != string::npos )
		iz = iz.substr(1, iz.size() - 2);
	iz = trimSpace(iz);
	string line = iz;
	if (iz[0] == '-') { operat = "-"; iz = iz.substr(1); } 
	if (iz.find_first_of("+*-/") == string::npos) {
		return iz;
	};
	string ret = "";
	while (iz != "") {
		string check = iz.substr(0, iz.find_first_of("*+-/()"));
		if (check == "") check = iz.substr(0, 1);
		if (check.find_first_of("+*-/()") != 0) {
			if (check == "R0" || check == "R1" || check == "R2" || check == "R3" || check == "R4" || check == "R5" || check == "R6" || check == "R7") {
				if (iz.find_first_of("+-/*()") == string::npos) iz = "";
				else iz = iz.substr(iz.find_first_of("+-/*()"));
				continue;
			}
			if (check.find_first_not_of("0123456789") == string::npos) {
				if (iz.find_first_of("+-/*()") == string::npos) iz = "";
				else iz = iz.substr(iz.find_first_of("+-/*()"));
				continue;
			}
			map<string, int>::iterator it;

			if ((*tabelaSimbola)[check]->getSek()->getNaziv == "UND") {
				if (iz.find_first_of("+-/*()") == string::npos) iz = "";
				else iz = iz.substr(iz.find_first_of("+-/*()"));
				continue;
			}
			if (oper == "*" || oper == "/") {
				greska = true;
				msg = "Pogresan operator!";
				break;
			}
			if (oper == "" || oper == "+") {
				int k = 0;
				for (; k < CrniPopravi.size() && CrniPopravi[k] != table[i]->getSection(); k++);
				plus[k]++;
			}
			if (oper == "-") {
				int k = 0;
				for (; k < CrniPopravi.size() && CrniPopravi[k] != table[i]->getSection(); k++);
				minus[k]++;
			}
			oper = "";
			if (iz.find_first_of("+-/*()") == string::npos) iz = "";
			else iz = iz.substr(iz.find_first_of("+-/*()"));
			continue;
		}
		if (check != ")") {
			if (check == "*" || check == "/" || check == "(") {
				if (check == "*") {
					operat = "*";
				}
				if (check == "/") {
					operat = "/";
				}
				if (check == "(") {
					operat = "";
				}
				iz = iz.substr(iz.find_first_of("/*(") + 1);
			}
			else {
				if (check == "+") operat = "+";
				if (check == "-") operat = "-";
				iz = iz.substr(iz.find_first_of("+-") + 1);
				continue;
			}
		}
		else {
			iz = iz.substr(iz.find_first_of(")") + 1);
		}
	}
		return operand;
}
*/
Parser::~Parser()
{
}


bool Parser::isChar(string s)
{
	if (s.size() >1) return false;

	switch (s[0]) {
	case '+': return true;
	case '-': return true;
	case '*': return true;
	case '/': return true;
	case '(': return true;
	case ')': return true;
	default: return false;
	}
}

int Parser::solve(int op1, int op2, char ch)
{
	switch (ch)
	{
	case '+': return (op1 + op2);
	case '-': return (op1 - op2);
	case '*': return (op1*op2);
	case '/': return (op1 / op2);
	default: return 0;
	}
}

int Parser::preference(char ch)
{
	switch (ch)
	{
	case '+': return 1;
	case '-': return 1;
	case '*': return 2;
	case '/': return 2;
	case '(': return -1;
	default: return 0;
	}
}

string Parser::toDecimal(string s1) {
	while (s1.find("0x") != string::npos) {
		size_t start = s1.find("0x");

		string sub = s1.substr(start);

		size_t end = sub.find_first_not_of("0x123456789ABCDEFabcdef");

		sub = sub.substr(0, end);
		int val = strtol(sub.c_str(), NULL, 16);

		stringstream ss;
		ss << val;
		string str = ss.str();

		s1 = s1.replace(start, sub.length(), str);
	}

	while (s1.find("0b") != string::npos) {
		size_t start = s1.find("0b");

		string sub = s1.substr(start);
		sub = sub.substr(2);


		size_t end = sub.find_first_not_of("01");

		sub = sub.substr(0, end);
		int val = strtol(sub.c_str(), NULL, 2);

		stringstream ss;
		ss << val;
		string str = ss.str();

		s1 = s1.replace(start, sub.length() + 2, str);
	}

	return s1;
}

int Parser::evaluate(string s1) //ovo treba da se menja, smeju i konstante
{
	s1 = toDecimal(s1);
	if (s1.find_first_not_of("0123456789()+-/* ") != string::npos) {
		cout << "Greska   " << s1 << endl;
		getchar();
		exit(1);
	}
	stack<char> operators;
	stack<int> operands;

	string s;
	istringstream is(s1);
	while (is >> s)
	{
		if (isChar(s))
		{
			if (s[0] == '(') operators.push('(');
			else if (s[0] == ')')
			{
				while (operators.top() != '(')
				{
					char ch = operators.top(); operators.pop();
					int op2 = operands.top(); operands.pop();
					int op1 = operands.top(); operands.pop();

					operands.push(solve(op1, op2, ch));
				}
				operators.pop();
			}
			else
			{
				while (!operators.empty() && preference(s[0]) <= preference(operators.top()))
				{
					char ch = operators.top(); operators.pop();
					int op2 = operands.top(); operands.pop();
					int op1 = operands.top(); operands.pop();

					operands.push(solve(op1, op2, ch));

				}

				operators.push(s[0]);
			}

		}
		else
		{
			int op = atoi(s.c_str());
			operands.push(op);
		}

	}

	while (!operators.empty())
	{
		char ch = operators.top(); operators.pop();
		int op2 = operands.top(); operands.pop();
		int op1 = operands.top(); operands.pop();

		operands.push(solve(op1, op2, ch));
	}

	//cout<<operands.empty()<<" "<<operators.empty();
	int result = operands.top();
	return result;
}
