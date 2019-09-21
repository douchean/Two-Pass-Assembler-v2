#pragma once

#include <vector>
#include <string>
#include <map>
#include "simbol.h"
#include "sekcija.h"
#include <cctype>
#include <stack>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <cstdio>

using namespace std;

class Parser
{

public:
	
	Parser(string izraz, map<string, Simbol*>* tabela) {
		sekcija = new Sekcija("UND");
		simbolZaRelokaciju = NULL;
		trebaRelokacija = false;
		this->izraz = izraz;
		this->tabelaSimbola = tabela;
		greska = false;
		msg = "";
	}

	vector<string> tokenizuj(string s) {
		vector<string> result;
		string op = "";
		int open = 0; bool switcher = false; //0 operand 1 operator
		bool space = true;
		char prethodni = '0';
		for (unsigned i = 0; i < s.length(); i++) {
			if (open < 0) {
				greska = true;
				msg = "Greske sa zagradama!";
				break;
			}
			if (s[i] == ' ') {
				if (space) continue;
				space = true;
				if (switcher == false) {
					result.push_back(op);
					op = "";
				}
				continue;
			}
			else {
				space = false;
			}
			if (s[i] == '-' || s[i] == '+' || s[i] == '*' || s[i] == '/') {
				if (prethodni == '-' || prethodni == '+' || prethodni == '*' || prethodni == '/') {
					greska = true;
					msg = "Vise uzastopnih operatora!";
					break;
				}
				result.push_back(op);
				op = "";
				operatori.push_back(s[i]);
				switcher = true; prethodni = s[i];
				continue;
			}
			if (s[i] == '(') {
				if (switcher == false) {
					greska = true;
					msg = "Greska sa otvaranjem zagrade! ";
					break;
				}
				open++;
				operatori.push_back(s[i]);
				prethodni = s[i];
				continue;
			}
			if (s[i] == ')') {
				if (switcher == true) {
					greska = true;
					break;
				}
				result.push_back(op);
				op = "";
				open--;
				operatori.push_back(s[i]);
				prethodni = s[i];
				continue;
			}
			op = op + s[i];
			prethodni = s[i];
			switcher = false;
		}
		if (op != "" && op != ")") result.push_back(op);

		return result;
	} //topkenizacija izraza

	bool isNumber(std::string s);

	bool isOperand(const std::string s);

	void checkSymbols();
	void checkSymbols(Sekcija* sekcija);


	static inline string trimSpace(string str) { // trimes the white space ----->checked, possibly check again
		size_t first = str.find_first_not_of(" \n\r\t");
		if (first == string::npos)
			return "";
		size_t last = str.find_last_not_of(" \n\r\t"); //tab dodat ***check
		if (last == string::npos) return "";
		str = str.substr(0, last + 1);
		str = str.substr(first);
		return str;
	}
	
	string getMsg() {
		return msg;
	}
	bool getGreska() {
		return greska;
	}
	bool trebaRel() { return trebaRelokacija;  }
	string getIzraz() { return izraz; }

	~Parser();

	string relok(string izraz);

	static bool isChar(string s);
	static int solve(int op1, int op2, char ch);
	static int preference(char ch);
	static int evaluate(string s1);
	static string toDecimal(string s1);

	Simbol* getSimbolZaRelokaciju() { return simbolZaRelokaciju;  }

	vector<Simbol*> getKonstante() { return konstante; }
	void setKonstante(vector<Simbol*> kon) { konstante = kon; }


private:
	vector<char> operatori;
	string izraz;
	map<string, Simbol*>* tabelaSimbola;
	string msg;
	bool greska;

	Sekcija* sekcija;
	bool trebaRelokacija;
	Simbol* simbolZaRelokaciju;

	vector<Simbol*> konstante;
};

