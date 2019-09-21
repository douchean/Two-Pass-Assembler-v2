#pragma once
#include <string>
#include <map>
#include<vector>
#include "sekcija.h"
#include "simbol.h"
#include "relokacija.h"
#include <fstream>
#include "Parser.h"
#include <iostream>
#include "Instrukcija.h"
#include <stdlib.h>

using namespace std;
class DrugiProlaz
{
private:
	Sekcija* sekcija; 
	long adresa; //trenutna adresa
	map<string, Simbol*> tabelaSimbola;
	vector<Simbol*> konstante;


	Instrukcija* inst;
	string adresiranjeSrc;
	string adresiranjeDst;
	string dodatno; //prosirenje na sta se odnosi

	
	string src;
	string dst;

	string regSrc, regDst; //koristimo ih za regind

	string opcode;

	string type;
	string second;
	//int r;
	bool pcrel1;
	bool pcrel2;
	bool pcrel;

	

	bool greska;
	string msg;

	vector<Sekcija*> sekcije;


public:

	string imeUlaz;
	int tipAdresiranja(string line);


	void extendInstruction(string simbol, int prosirenja, bool pcrel, int oduzimanje);
	int duzinaInstrukcije(string line);
	void obradiInstrukciju(string ins);
	void prolaz(map<string, Simbol*> tabela, vector<Sekcija*>& sekcije);




	void ubaciHex(string hexa);
	void putCode(string code) {
		sekcija->addCode(code);
	}
	string binToHex(string bin);
	string toHex(int value){
		stringstream stream;
		stream << hex << value;
		string result(stream.str());

		return result;
	}
	string longToHex(long value) {
		stringstream stream;
		stream << hex << value;
		string hexa(stream.str());

		int lengthBin = hexa.length();
		if (lengthBin < 4) {
			for (int i = 0; i < 4 - lengthBin; i++) {
				hexa = "0" + hexa;
			}
		}
		return hexa;
	}
	string registerCode(string reg) {

		if (reg == "r0") return "0000";
		if (reg == "r1") return "0001";
		if (reg == "r2") return "0010";
		if (reg == "r3") return "0011";
		if (reg == "r4") return "0100";
		if (reg == "r5") return "0101";
		if (reg == "r6") return "0110";
		if (reg == "r7") return "0111";
		if (reg == "pc") return "0111";
		if (reg == "sp") return "0110";
		if (reg == "psw") return "1111";

		return "1111";
	}

	void relokacija(long adresa, bool pcrel, int num);

	int trebaRelokacija(); //0 ne treba, 1 treba za dst, 2 treba za src
	/*vector<vector<string>> getCode() {
		return kod;
	}*/


	void generisiInstr(string line, int adresa);

	void dotChar(int line);
	void dotWord(int line);
	void dotLong(int line);
	//void dotAlign(int line);
	//void dotSkip(int line);

	void add(string line, long adr, int prosirenja);
	void sub(string line, long adr, int prosirenja);
	void mul(string line, long adr, int prosirenja);
	void div(string line, long adr, int prosirenja);
	void cmp(string line, long adr, int prosirenja);
	void andd(string line, long adr, int prosirenja);
	void orr(string line, long adr, int prosirenja);
	void nott(string line, long adr, int prosirenja);
	void test(string line, long adr, int prosirenja);
	void push(string line, long adr, int prosirenja);
	void pop(string line, long adr, int prosirenja);
	void call(string line, long adr, int prosirenja);
	void iret(string line, long adr, int prosirenja);
	void mov(string line, long adr, int prosirenja);
	void shl(string line, long adr, int prosirenja);
	void shr(string line, long adr, int prosirenja);
	void ret(string line, long adr, int prosirenja);
	void jmp(string line, long adr, int prosirenja);
	void jeq(string line, long adr, int prosirenja);
	void jne(string line, long adr, int prosirenja);
	void jgt(string line, long adr, int prosirenja);
	void halt(string line, long adr, int prosirenja);
	void xchg(string line, long adr, int prosirenja);
	void intt(string line, long adr, int prosirenja);
	void xorr(string line, long adr, int prosirenja);

	DrugiProlaz();
	~DrugiProlaz();

	vector<string> tokenize(string s, char c);
	string trimComm(string line);
	string trimSpace(string str);
	bool isRegister(string str); 
	bool isRegisterLowHigh(string reg);
	bool isNumber(string str);
};

