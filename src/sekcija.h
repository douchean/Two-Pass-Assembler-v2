#ifndef _sekcija_h_
#define _sekcija_h_

#include <vector>
#include <string>
#include "relokacija.h"
using namespace std;

class Sekcija 
{
public:
	Sekcija(string naziv) { this->naziv = naziv; enter = 0;}

	Sekcija();
	~Sekcija();

	//void setAdresa(long adr) { adresa = adr; }
	//long getAdresa() { return adresa;  }

	void setNaziv(string naziv) { this->naziv = naziv; }
	string getNaziv() { return naziv;  }

	void addCode(string code) {
		kod.push_back(code);
		enter++;
		if(enter < 16)
		kod.push_back(" ");
		else {
			kod.push_back("\n");
			enter = 0;
		}
	}
	vector<Relokacija*> getTabelaRelokacija() { return tabelaRelokacija;  }
	vector<string> kod;
	vector<Relokacija*> tabelaRelokacija;
private:
	int enter;
	//long adresa;
	string naziv;
};

#endif
