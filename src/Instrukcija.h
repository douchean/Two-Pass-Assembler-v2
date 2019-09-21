#pragma once
#include <string>

using namespace std;
class Instrukcija{
		/*•	Neposredno adresiranje: #konstantan_izraz
		•	Registarsko direktno: Ri
		•	Memorijsko direktno: adresa; adresa je konstantan izraz.
		•	Registarsko indirektno: [Ri]
		•	Registarsko indirektno sa pomerajem: Ri[offset]; offset je konstantan iztaz.
		*/
		
public:
	string ime;
	int brParam;
	string opcode;
	Instrukcija(string i, int m, string oc) {
		ime = i; brParam = m; opcode = oc;
		//nema max min samo broj
		//ret preveemo kao pop i nema argument, prevodi se kao pop kome je src pc
		//jmp pseudoinstr prevodi se kao add ako je pcrel $ ili mov ako je bilo koje drugo
	}
	~Instrukcija();
};

