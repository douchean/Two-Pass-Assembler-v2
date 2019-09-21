#ifndef _simbol_h_
#define _simbol_h_

#include <string>
#include "sekcija.h"
class Simbol
{
public:
	Simbol();
	~Simbol();
	bool isDefined();
	void setNaziv(std::string naziv) { this->naziv = naziv; }
	void setLocal() { global = false; }
	void setGlobal() { global = true; }
	bool isGlobal();
	void setExtern() { extrn = true; global = true; }
	bool isExtern();
	void setConst() { con = true; }
	bool isConst();
	void setVrednost(long value) { vrednost = value; }// nije potreban
	long getVrednost() { return vrednost; }

	std::string getNaziv() { return naziv; }
	//void setRelativan() { tip = 1; }
	//void setApsolutan() { tip = 2; }
	//void setNepoznat() { tip = 3; }
	void setDef(bool x) { def = x; }
	void setIzraz(std::string code) { izraz = code; }
	//bool isAps() { if (tip == 2) return true; else return false; }
	//bool isRel() { if (tip == 1) return true; else return false; }
	void setRbr(int rbr0){ rBroj = rbr0;  }
	int getRbr() { return rBroj;  }
	void setSek(Sekcija* s) { sekcija = s; }
	Sekcija* getSek() { return sekcija;  }

private:
	std::string naziv;
	bool global; //1 globalni 0 lokalni
	bool extrn;
	bool def; //da li je definisan, ako je global==1 mora i def biti 1
	bool con; //da li je simbol konstanta
	//da li je sekcija ili simbol
	//int tip; //relativan 1 apsolutan 2 nepoznat 3 neispravan 4
	//vrednost u izvedenim
	//labele: adresa prve sledece stvari; sekcije: ako je pre nje org onda je vrednost i z orga ako ne onda 0
	//izrazi: ako je a + b ili a def 3 ovi sa def uvek imaju izraz, labele i sekcije uvek imaju vrednost
	int rBroj; //racunam ga na kraju
	long vrednost; //kod labela, adresa prve sledece stvari, sekcije
	//ako imaju org direktivu vrednosti iz orga, ako nemaju onda nula
	//izraz vrednost izraza
	std::string izraz;
	Sekcija* sekcija;
};

#endif
