#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include<vector>
#include <stack>

#include "simbol.h"
#include "sekcija.h"
#include "Parser.h"
#include "relokacija.h"
#include "drugiProlaz.h"
#include "Instrukcija.h"

using namespace std;

bool isNumber(std::string s) {
	if (s.find("0x") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("0123456789ABCDEFabcdef") == string::npos)
			return true;
		else
			return false;
	} else if (s.find("0b") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("01") == string::npos)
			return true;
		else
			return false;
	}
	if (s.find_first_not_of("0123456789") == string::npos)
		return true;
	return false;
}

const string registri[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
		"pc" "sp", "psw" };

bool isRegister(string reg) {
	if (reg == "r0")
		return true;
	if (reg == "r1")
		return true;
	if (reg == "r2")
		return true;
	if (reg == "r3")
		return true;
	if (reg == "r4")
		return true;
	if (reg == "r5")
		return true;
	if (reg == "r6")
		return true;
	if (reg == "r7")
		return true;
	if (reg == "pc")
		return true;
	if (reg == "sp")
		return true;
	if (reg == "psw")
		return true;
	return false; //nije reg
} //moze bool ako mi ne treba 

bool isRegisterLowHigh(string reg) {
	if (reg == "r0l")
		return true;
	if (reg == "r1l")
		return true;
	if (reg == "r2l")
		return true;
	if (reg == "r3l")
		return true;
	if (reg == "r4l")
		return true;
	if (reg == "r5l")
		return true;
	if (reg == "r6l")
		return true;
	if (reg == "r7l")
		return true;

	if (reg == "r0h")
		return true;
	if (reg == "r1h")
		return true;
	if (reg == "r2h")
		return true;
	if (reg == "r3h")
		return true;
	if (reg == "r4h")
		return true;
	if (reg == "r5h")
		return true;
	if (reg == "r6h")
		return true;
	if (reg == "r7h")
		return true;
	return false;
}

static vector<Instrukcija*> initialize() {
	Instrukcija* ins;
	/* izmeniti oc i flegove i instrukcije*/
	//zapravo ne sme da se koristi imm za ove koje smestaju nesto u dst
	vector<Instrukcija*> instrukcije;
	instrukcije.push_back(ins = new Instrukcija("halt", 0, "1"));
	instrukcije.push_back(ins = new Instrukcija("xchg", 2, "2"));
	instrukcije.push_back(ins = new Instrukcija("int", 1, "3"));
	instrukcije.push_back(ins = new Instrukcija("mov", 2, "4"));
	instrukcije.push_back(ins = new Instrukcija("add", 2, "5"));
	instrukcije.push_back(ins = new Instrukcija("sub", 2, "6"));
	instrukcije.push_back(ins = new Instrukcija("mul", 2, "7"));

	instrukcije.push_back(ins = new Instrukcija("div", 2, "8"));
	instrukcije.push_back(ins = new Instrukcija("cmp", 2, "9"));
	instrukcije.push_back(ins = new Instrukcija("not", 1, "A"));

	instrukcije.push_back(ins = new Instrukcija("and", 2, "B"));
	instrukcije.push_back(ins = new Instrukcija("or", 2, "C"));

	instrukcije.push_back(ins = new Instrukcija("xor", 2, "D"));
	instrukcije.push_back(ins = new Instrukcija("test", 2, "E"));

	instrukcije.push_back(ins = new Instrukcija("shl", 2, "F")); //samo labelae
	instrukcije.push_back(ins = new Instrukcija("shr", 2, "10"));

	instrukcije.push_back(ins = new Instrukcija("push", 1, "11"));

	instrukcije.push_back(ins = new Instrukcija("pop", 1, "12"));
	instrukcije.push_back(ins = new Instrukcija("jmp", 1, "13"));

	instrukcije.push_back(ins = new Instrukcija("jeq", 1, "14"));
	instrukcije.push_back(ins = new Instrukcija("jne", 1, "15"));

	instrukcije.push_back(ins = new Instrukcija("jgt", 1, "16"));
	instrukcije.push_back(ins = new Instrukcija("call", 1, "17"));

	instrukcije.push_back(ins = new Instrukcija("ret", 0, "18"));
	instrukcije.push_back(ins = new Instrukcija("iret", 0, "19"));
	//ret se prevodi kao pop r7(pc)

	//jmp se prevodi u zav od tipa adresiranja, pcrel je add r7, pomeraj, od adrese lab na koju se skace se oduzme pc i dobije pom
	//ili kao mov r7, labela
	/*
	 */
	return instrukcije;
}

void registerInstructionCheck(int brLinije, string ins) {
	if (ins == "call" || ins.find_first_of("j") == 0 || ins == "int") {
		cout << "Greska na liniji: " << brLinije << endl;
		cout << "Nije dozvoljeno registarsko direktno adresiranje za instrukciju " << ins << endl;
		exit(-1);
	}
}

static inline string trimSpace(string& str) { // trimes the white space ----->checked, possibly check again
	size_t first = str.find_first_not_of(" \n\r\t");
	if (first == string::npos)
		return "";
	size_t last = str.find_last_not_of(" \n\r\t"); //tab dodat ***check
	if (last == string::npos)
		return "";
	str = str.substr(0, last + 1);
	str = str.substr(first);
	return str;
}

static inline string trimComm(string& line) { //trims the comment  -----> checked
	size_t size = line.find_first_of(';');
	if (line.npos != size) {
		line = line.substr(0, size);
	}
	return line;
}

vector<string> tokenize(string s, char c) {
	vector<string> result;

	// Skip delimiters at beginning.
	string::size_type lastPos = s.find_first_not_of(c, 0);

	// Find first non-delimiter.
	string::size_type pos = s.find_first_of(c, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		result.push_back(s.substr(lastPos, pos - lastPos));

		// Skip delimiters.
		lastPos = s.find_first_not_of(c, pos);

		// Find next non-delimiter.
		pos = s.find_first_of(c, lastPos);
	}
	return result;
}

int brLinije = 0;

static vector<Simbol*> konstante;
static vector<Simbol*> nisuKonstante;

map<string, Simbol*> prviProlaz(string imeUlaz, vector<Sekcija*>& sekcije) {
	string und = "UND";
	ifstream myfile; //labele definicije instrukcije
	myfile.open(imeUlaz.c_str());
	if (!myfile.is_open()) {
		std::cout << "Nije otvoren ulazni fajl";
		getchar();
		exit(-1);
	}
	myfile.seekg(0, ios::beg);

	vector<Instrukcija*> instrukcije = initialize();
	map<string, Simbol*> tabela;
	string line;
	Sekcija* sekcija = new Sekcija();
	sekcija->setNaziv(und);
	long adresa = 0; //trenutna adresa

	string rest;
	bool greska = false;
	bool ended = false;
	string msg;

	while (getline(myfile, line)) {
		line = trimComm(line);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		line = trimSpace(line);
		rest = line;

		//finding a label

		//obraditi greske, da li ima vise':' ili da pocinje naziv labele sa org itd
		//ne bi trebalo labela pa sekcija ili labela pa global
		//samo jednom moye global na pocetku
		if (line.empty())
			continue;
		if (rest.find(".global") == 0) {

			brLinije++;
			rest = rest.substr(7);
			if (rest.find(" ") != 0) { //.globalitd
				greska = true;
				msg = "Pogresan unos!";
				break;
			}
			vector<string> tokens = tokenize(rest, ',');
			for (vector<string>::iterator it = tokens.begin();
					it != tokens.end(); ++it) {
				(*it) = trimSpace(*it);
				if (tabela.find(*it) != tabela.end()) {
					greska = true;
					msg = "Simbol vise puta definisan kao globalan!";
					break;
				} else {
					if ((*it).find_first_of(" ") != string::npos) {
						greska = true;
						msg = "Nije dozvoljen space u nazivu labele!";
						break;
					}
					Simbol* sim = new Simbol();
					sim->setGlobal();
					sim->setDef(false);
					sim->setSek(new Sekcija("UND"));
					sim->setVrednost(0);
					sim->setNaziv(*it);
					sim->setRbr(tabela.size() + 1);
					tabela[*it] = sim;
				}
			}
			continue;
		} 
		else if (rest.find(".extern") == 0) {

			brLinije++;
			rest = rest.substr(7);
			if (rest.find(" ") != 0) { //.externitd
				greska = true;
				msg = "Pogresan unos!";
				break;
			}
			vector<string> tokens = tokenize(rest, ',');
			for (vector<string>::iterator it = tokens.begin();
					it != tokens.end(); ++it) {
				(*it) = trimSpace(*it);
				if (tabela.find(*it) != tabela.end()) {
					greska = true;
					msg = "Eksterni simbol vise puta unesen!";
					break;
				} else {
					if ((*it).find_first_of(" ") != string::npos) {
						greska = true;
						msg = "Nije dozvoljen razmak u nazivu labele!";
						break;
					}
					Simbol* sim = new Simbol();
					sim->setExtern();
					sim->setDef(false);
					sim->setSek(new Sekcija("UND"));
					sim->setVrednost(0);
					sim->setNaziv(*it);
					sim->setRbr(tabela.size() + 1);
					tabela[*it] = sim;
				}
			}
			continue;
		} 
		else if (line.find_first_of(":") != string::npos) {

			size_t pos = line.find_first_of(":");
			brLinije++;
			if (sekcija == NULL || sekcija->getNaziv() == "UND") {
				greska = true;
				msg = "Greska: nije dozvoljeno definisati simbol bez sekcije!";
				break;
			}
			string name = line.substr(0, pos);
			name = trimSpace(name);
			/*
			 if (isRegister(name)) {
			 greska = true;
			 msg = "Naziv simbola ne moze biti naziv registra!";
			 break;
			 }*/
			if (name.empty() || name.find_first_of(" :	") != string::npos
					|| isNumber(name) || isRegister(name)
					|| isRegisterLowHigh(name)) {
				greska = true;
				msg = "Greska prilikom davanja naziva labeli!";
				break;
			}
			Simbol* lab; //ako ima izraz onda nije labela vec simbol
			if (tabela.find(name) != tabela.end()) {
				if ((*tabela[name]).isDefined()) {
					greska = true;
					msg = "Trazeni simbol vec postoji u tabeli!";
					break;
				} // vec postoji simbol
				else {
					tabela[name]->setDef(true);
					tabela[name]->setSek(sekcija);
					tabela[name]->setVrednost(adresa);
					//sekcija->addSimbol(tabela[name]);
				}
			} else {
				lab = new Simbol();
				lab->setNaziv(name);
				lab->setSek(sekcija);
				lab->setLocal();
				//lab->setRelativan();
				lab->setVrednost(adresa); //nema izraz
				lab->setDef(true);
				lab->setRbr(tabela.size() + 1);
				tabela[name] = lab;
				//sekcija->addSimbol(lab);
			}
			//label done
			rest = line.substr(pos + 1);
			rest = trimSpace(rest);
			if (!rest.empty())
				brLinije--;
		} //ne treba

		else if (rest.find(".text") == 0 || rest.find(".data") == 0
				|| rest.find(".bss") == 0) { //sekcija
			brLinije++;
			//if (sekcija != NULL) sekcija->setVelicina(adresa);
			//naziv sekcije .tet i sl je naziv sekcije
			//ako ima nesto posle naziva greska bye
			string naziv;
			if (rest.find(".text") == 0) {
				naziv = ".text";
				rest = rest.substr(5);
			} else if (rest.find(".data") == 0) {
				naziv = ".data";
				rest = rest.substr(5);
			} else if (rest.find(".bss") == 0) {
				naziv = ".bss";
				rest = rest.substr(4);
			}
			if (!rest.empty()) { //eliminise slucaj da linija pocinje sa .texts i sl.
				greska = true;
				msg = "pogresno nazvana sekcija!";
				break;
			}
			adresa = 0;
			sekcija = new Sekcija();
			sekcija->setNaziv(naziv);
			if (tabela.find(naziv) != tabela.end())
				if ((*tabela[naziv]).isDefined()) {
					greska = true;
					msg = "Visestruko definisanje iste sekcije";
					break;
				} // vec postoji sekcija
				else {
					tabela[naziv]->setDef(true);
					(tabela[naziv])->setSek(sekcija);
					tabela[naziv]->setVrednost(0);
					//sekcija->addSimbol(tabela[name]);
				}
			else {
				Simbol* simb = new Simbol();
				simb->setNaziv(naziv);
				simb->setSek(sekcija);
				simb->setLocal();
				simb->setVrednost(0);
				simb->setDef(true);
				simb->setRbr(tabela.size() + 1);
				tabela[naziv] = simb;
			}
			if (rest.find_first_not_of("	 ") != string::npos) {
				greska = true;
				msg = "u redu sa sekcijom ima jos karaktera";
				break;
			}
			sekcije.push_back(sekcija);
			continue;
		} 
		else if (rest.find(".section") == 0) { //sekcija2
			brLinije++;
			//if (sekcija != NULL) sekcija->setVelicina(adresa);
			//naziv sekcije .tet i sl je naziv sekcije
			//ako ima nesto posle naziva greska bye
			string naziv;

			rest = rest.substr(8);

			if (rest.find(" ") != 0) { //.sectionz
				greska = true;
				msg = "Pogresan unos!";
				break;
			}

			rest = trimSpace(rest);

			vector<string> tokens = tokenize(rest, ',');
			vector<string>::iterator it = tokens.begin();
			(*it) = trimSpace(*it);

			naziv = (*it);
			if (naziv.empty()
					|| naziv.find_first_of(" :	!@#$%^&*()") != string::npos
					|| isNumber(naziv) || isRegister(naziv)
					|| isRegisterLowHigh(naziv)) {
				greska = true;
				msg = "Greska prilikom davanja naziva sekciji!";
				break;
			}

			adresa = 0;
			sekcija = new Sekcija();
			sekcija->setNaziv(naziv);
			if (tabela.find(naziv) != tabela.end()) {
				if ((*tabela[naziv]).isDefined()) {
					greska = true;
					msg = "Visestruko definisanje iste sekcije";
					break;
				} // vec postoji sekcija
				else {
					tabela[naziv]->setDef(true);
					(tabela[naziv])->setSek(sekcija);
					tabela[naziv]->setVrednost(0);
					//sekcija->addSimbol(tabela[name]);
				}
			} else {
				Simbol* simb = new Simbol();
				simb->setNaziv(naziv);
				simb->setSek(sekcija);
				simb->setLocal();
				simb->setVrednost(0);
				simb->setDef(true);
				simb->setRbr(tabela.size() + 1);
				tabela[naziv] = simb;
			}
			sekcije.push_back(sekcija);
			continue;
		} 
		else if (rest.find(".end") == 0) {
			ended = true;
			//if (sekcija != NULL) sekcija->setVelicina(adresa);
			rest = rest.substr(4);
			if (!rest.empty()) {
				greska = true;
				msg = "Da li ste mislisi '.end'?";
			}
			break;
		} //nema
		else if (rest.find(".equ") == 0) {
			brLinije++;
			rest = rest.substr(4);

			size_t pos = rest.find(",");
			if (sekcija == NULL || sekcija->getNaziv() == "UND") {
				greska = true;
				msg =
						"Greska: nije dozvoljeno definisati konstantu bez sekcije!";
				break;
			}

			if (pos == string::npos) {
				greska = true;
				msg = "Direktiva ekvivalencija zahteva unetu vrednost!";
				break;
			}
			string ime = rest.substr(0, pos);
			string izraz = rest.substr(pos + 1);

			ime = trimSpace(ime);
			if (ime.empty()
					|| ime.find_first_of(" :	!@#$%^&*()") != string::npos
					|| isNumber(ime) || isRegister(ime)
					|| isRegisterLowHigh(ime)) {
				greska = true;
				msg = "Greska prilikom davanja naziva sekciji!";
				break;
			}
			Simbol* kon;
			if (tabela.find(ime) != tabela.end()) {
				if ((*tabela[ime]).isDefined()) {
					greska = true;
					msg = "Trazeni simbol vec postoji u tabeli!";
					break;
				} // vec postoji simbol
				else { //podesiti mu vrednost
					tabela[ime]->setDef(true);
					tabela[ime]->setSek(sekcija); //da li?
					tabela[ime]->setConst();
					kon = tabela[ime];
				}
			} else {
				kon = new Simbol();
				kon->setNaziv(ime);
				kon->setSek(sekcija); //da li?
				kon->setLocal();
				kon->setDef(true);
				kon->setRbr(tabela.size() + 1);
				kon->setConst();
				tabela[ime] = kon;
			}

			izraz = trimSpace(izraz);
			kon->setIzraz(izraz);

			int value = Parser::evaluate(izraz); //implementirati, trebalo bi da samo konstante smiju
			if (value == -1) {
				greska = true;
				msg = "Nekorektan format izraza!";
				break;
			} else
				kon->setVrednost(value);
			continue;
			//greska

			//da li je samo jedan zavisni simbol
			//ako ih je vise, onda je greska
			//else dohvati sekciju tog simbola, ta sekcija je sekcija novog
			//dodati ga u tabelu simbola lokalni izraz i naziv setovani
			//50ak minuta

		} //char word long
		if (rest.find(".word") == 0 || rest.find(".byte") == 0) {
			brLinije++;
			if (sekcija->getNaziv() == "UND" || sekcija->getNaziv() == ".bss") {
				greska = true;
				msg =
						"Greska: nije dozvoljeno koristiti ovu direktivu u okviru sekcije "
								+ sekcija->getNaziv();
				break;
			}
			int bytes;
			if (rest.find(".byte") == 0) {
				bytes = 1;
				rest = rest.substr(rest.find(".byte") + 5);
			} else if (rest.find(".word") == 0) {
				bytes = 2;
				rest = rest.substr(rest.find(".word") + 5);

			}/* else if (rest.find(".char") == 0) {
			 bytes = 1;
			 rest = rest.substr(rest.find(".char") + 5);

			 }*/
			if (rest.find_first_of(" \r\t") != 0) {
				greska = true;
				msg = "Greska u nazivu!";
				break;
			}

			//rest.substr(2);
			vector<string> tokens = tokenize(rest, ',');
			int delovi = 0;
			for (vector<string>::iterator it = tokens.begin();
					it != tokens.end(); ++it) {
				(*it) = trimSpace(*it);

				if (!(*it).empty()) {
					Parser* parser = new Parser(*it, &tabela);
					parser->checkSymbols();
					if (parser->getGreska()) {
						std::cout << "Greska na liniji: " << brLinije << endl;
						std::cout << parser->getMsg() << endl;
						getchar();
						exit(-1);
					}

					if (!parser->getKonstante().empty()) {
						vector<Simbol*> con = parser->getKonstante();
						for (vector<Simbol*>::iterator itt = con.begin();
							itt != con.end(); ++itt) {
							konstante.push_back(*itt);
						}
					}
					//parsiraj pre
					//ako nije nepoznat parsiraj posle
					//upisi broj bajtova
				} else {
					greska = true;
					msg = "Praznina izmedju zapeta";
					break;
				}
				delovi++;
				//za jedan izraz jedna relokacija
				//.word a+4-5+7, 4+6, 2, d - a + 7 
				//labela samo prva ili prva dva ako su u istoj sekciji
				//pa da li je u tab simb, ako ima nista
				//ako nema ubaciti i sekcija = undefined
				//ako se ne definise posle kraja 1og greska
				//lc += 2* br tokena
				//da li sekcija restaurira
				//i lebele u tabelu simbola kao nedefinisane, kad nadjem 
				// sto se ostatka tice zanima me imam li labelu jos u prolazu
				//c++ algoritam koji racuna koji je dat u stringu, skinuti sa neta
				//infix u postfix,oni rade samo sa jednocifrenim pa bi mogla
				//modifikacija da radi sa dvocifrenima

				/*vector<string> */
				//if izraz apsolutan if nije greska
				//naci vrednost izraza tokena i dodati na prethodni
				//znaci ako je 2 dup 5, 3 dup a + b, a + 3 dup 7
				//onda je to 2 + 3 + a + 3 ) * bytes a a je apsolutno
				//i to dodamo na adresu
				//ono sa desne strane dupa upamtiti u niz i nek to bude jedan red sekcije
				//a sekcija da ima niz redova tj dve vrste jedna podaci drugi instrukcije
				//i moze da pamti 1 2 ili 4 za jedan red podataka
				//poslusati ponovo 1 cas na 1 sat
				//zbit tih svih brojeva puta velicina je koliko treba povecati adresu
			}
			adresa += delovi * bytes;
		} 
		else if (rest.find(".align") == 0 || rest.find(".skip") == 0) {
			brLinije++;
			if (sekcija->getNaziv() == "UND"
					|| sekcija->getNaziv() == ".text") {
				greska = true;
				msg =
						"Greska: nije dozvoljeno koristiti ovu direktivu u okviru sekcije "
								+ sekcija->getNaziv();
				break;
			}
			bool align = false; //default skip
			if (rest.find(".align") == 0) {
				rest = rest.substr(rest.find(".align") + 6);
				align = true;
			} else if (rest.find(".skip") == 0) {
				rest = rest.substr(rest.find(".skip") + 5);
				align = false;
			}

			if (rest.find_first_of(" \r\t") != 0) {
				greska = true;
				msg = "pogresno nazvana direktiva";
				break;
			}
			rest = trimSpace(rest);
			int value = Parser::evaluate(rest); //implementirati, trebalo bi da samo konstante smiju
			if (value == -1) {
				greska = true;
				msg = "Nekorektan format izraza!";
				break;
			}

			if (align) {
				if (adresa == 0) continue;
				adresa = adresa - (adresa % value) + value;
			} else {
				adresa += value;
			}

		} 
		else if (!rest.empty()) {
			brLinije++;
			if (sekcija->getNaziv() == "UND" || sekcija->getNaziv() == ".bss"
					|| sekcija->getNaziv() == ".data") {
				greska = true;
				msg =
						"Greska: nije dozvoljeno pozivati instrukcije u okviru sekcije "
								+ sekcija->getNaziv();
				break;
			}
			rest = trimSpace(rest);
			//ispitati da li je instrukcija ako nije f off
			bool ins = false;
			for (vector<Instrukcija*>::iterator it = instrukcije.begin();
					it != instrukcije.end(); ++it) {
				//u zav od tipa adresiranja adresa += 1 do 7 bajtova
				//ako nema arg koji nije regdir onda 2 b inace je 4
				if (rest.find((*it)->ime) == 0) {
					string instructionName = (*it)->ime;
					ins = true;
					int bytes = 1;
					rest = rest.substr((*it)->ime.size());

					int prosirenja = 0; //provera koliko puta je potrebno prosirenje

					if (rest[0] == 'b') {
						prosirenja = 1;
						rest = rest.substr(1);
					} else if (rest[0] == 'w') {
						prosirenja = 2;
						rest = rest.substr(1);
					} else if (rest.find_first_of(" \r\t") != 0) {
						std::cout << "Greska na liniji: " << brLinije << endl;
						std::cout << "Pogresna instrukcija!" << endl;
						getchar();
						exit(-1);
					}

					rest = trimSpace(rest);
					int brTokena = 0;
					/*if(rest.find_first_not_of("	 ") == string::npos){
					 if((*it)->brParam != 0){
					 std::cout << "Greska na liniji: " << brLinije << endl;
					 std::cout << "Pogresan broj parametara instrukcije!" << endl;
					 getchar();
					 exit(-1);
					 }
					 adresa += 1;
					 break;
					 }*/

					vector<string> tokeni = tokenize(rest, ',');
					brTokena = tokeni.size(); //broj parametara

					int brojOperanda = 0;
					if (brTokena != (*it)->brParam) {
						std::cout << "Greska na liniji: " << brLinije << endl;
						std::cout << "Pogresan broj parametara instrukcije!"
								<< endl;
						getchar();
						exit(-1);
					}

					if (brTokena == 0) {
						adresa += 1;
						break;
					}

					for (vector<string>::iterator iter = tokeni.begin();
							iter != tokeni.end(); ++iter) {
						string cachedIter = (*iter);
						brojOperanda++;
						(*iter) = trimSpace(*iter);
						if ((*iter).find_first_of(" \r\t") != string::npos) {
							std::cout << "Greska na liniji: " << brLinije
									<< endl;
							std::cout << "Pogresan operand!" << endl;
							getchar();
							exit(-1);
						}

						if (isRegisterLowHigh(*iter)) {
							if (prosirenja == 1) {
								registerInstructionCheck(brLinije, instructionName);
								bytes += 1;
								continue;
							} 
							else {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Nizi ili visi biti registra koriste se iskljucivo za operand velicine jednog bajta!"
										<< endl;
								getchar();
								exit(-1);
							}
						} 
						else if (isRegister(*iter)) {
							registerInstructionCheck(brLinije, instructionName);
							bytes += 1;
							continue;
						}
						if ((*iter).find("[") != string::npos) { //regind
							if ((*iter).find("]") == (*iter).length() - 1) {

								if ((*iter).find("[") == 0) { //regind nopom
									string reg = (*iter).substr(1,
											(*iter).length() - 2);
									if (isRegister(reg)) {
										bytes += 1;
										continue;
									} else {
										std::cout << "Greska na liniji: "
												<< brLinije << endl;
										std::cout
												<< "Pogresan format registarskog indirektnog adresiranja bez pomeraja!"
												<< endl;
										getchar();
										exit(-1);
									}
								} 
								else {
									string reg = (*iter).substr(0,
											(*iter).find("["));
									if (isRegister(reg) && prosirenja > 0) { //regindpom
										(*iter) = (*iter).substr(
												reg.length() + 1,
												(*iter).length() - reg.length()
														- 2);
										if (!isNumber(*iter)) {
											if (isRegister(*iter) || isRegisterLowHigh(*iter)) {
												std::cout << "Greska na liniji: "
													<< brLinije << endl;
												std::cout
													<< "Pogresan je format za registarsko indirektno adresiranje sa pomerajem!" << *(iter)
													<< endl;
												getchar();
												exit(-1);
											}
											if (tabela.find(*iter)
													== tabela.end()) { //novi simbol, dodati u tabelu
												Simbol *lab = new Simbol();
												lab->setNaziv(*iter);
												lab->setLocal();
												lab->setDef(false);
												lab->setRbr(tabela.size() + 1);
												tabela[(*iter)] = lab;
											}
										}
										bytes += 1 + prosirenja;
										continue;
									} 
									else {
										std::cout << "Greska na liniji: "
												<< brLinije << endl;
										std::cout
												<< "Pogresan format registarskog indirektnog adresiranja sa pomerajem!"
												<< reg << endl;
										getchar();
										exit(-1);
									}
								}
							}
							std::cout << "Greska na liniji: " << brLinije
									<< endl;
							std::cout
									<< "Pogresan format registarskog indirektnog adresiranja!"
									<< endl;
							getchar();
							exit(-1);
						}
						if ((*iter).find_first_of("&") == 0) { //neposredno, sa simbolom --> proveriti da li sme
							(*iter) = (*iter).substr(1);
							if ((brojOperanda == 1 && (*it)->ime != "push")
									|| (brojOperanda == 2
											&& (*it)->ime == "xchg")) {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Neposredno adresiranje za dst operand nije dozvoljeno!"
										<< endl;
								getchar();
								exit(-1);
							}

							if (!isNumber(*iter) && !isRegister(*iter)
									&& !isRegisterLowHigh(*iter)) {
								if (tabela.find(*iter) == tabela.end()) { //novi simbol, dodati u tabelu
									Simbol *lab = new Simbol();
									lab->setNaziv(*iter);
									lab->setLocal();
									lab->setDef(false);
									lab->setRbr(tabela.size() + 1);
									tabela[(*iter)] = lab;
									nisuKonstante.push_back(lab);
								}
								else nisuKonstante.push_back(tabela[*iter]);
								bytes += 1 + prosirenja;
								continue;
							} 
							else {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Pogresan format neposrednog adresiranja!"
										<< endl;
								getchar();
								exit(-1);
							}
						}
						if (isNumber(*iter)) { //neposredno adresiranje sa oznacenim pomerajem, srediti

							if ((brojOperanda == 1 && (*it)->ime != "push")
									|| (brojOperanda == 2
											&& (*it)->ime == "xchg")) {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Neposredno adresiranje za dst operand nije dozvoljeno!"
										<< endl;
								getchar();
								exit(-1);
							}
							bytes += 1 + prosirenja;
							continue;
						}
						if ((*iter).find_first_of("*") == 0) {
							if (prosirenja != 2) {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Apsolutno adresiranje dozvoljeno samo za operand velicine dva bajta!"
										<< endl;
								getchar();
								exit(-1);
							}
							(*iter) = (*iter).substr(1);
							if (isNumber(*iter)) {
								bytes += 1 + prosirenja;
								continue;
							} else {
								if (tabela.find(*iter) == tabela.end()) { //novi simbol, dodati u tabelu
									Simbol *lab = new Simbol();
									lab->setNaziv(*iter);
									lab->setLocal();
									lab->setDef(false);
									lab->setRbr(tabela.size() + 1);
									tabela[(*iter)] = lab;
									konstante.push_back(lab);
								}
								else {
									konstante.push_back(tabela[*iter]);
								}
								bytes += 1 + prosirenja;
								continue;
							}
						}
						if ((*iter).find_first_of("$") == 0) { //pcrel moze samo za simbole
						(*iter) = (*iter).substr(1);
							if (!isNumber(*iter) && !isRegister(*iter)
									&& !isRegisterLowHigh(*iter) && prosirenja == 2) {
								if (tabela.find(*iter) == tabela.end()) { //novi simbol, dodati u tabelu
									Simbol *lab = new Simbol();
									lab->setNaziv(*iter);
									lab->setLocal();
									lab->setDef(false);
									lab->setRbr(tabela.size() + 1);
									tabela[(*iter)] = lab;
									nisuKonstante.push_back(lab);
								} else {
									nisuKonstante.push_back(tabela[*iter]);
								}
								bytes += 1 + prosirenja;
								continue;
							} else {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout
										<< "Pogresan format pcrel adresiranja!"
										<< endl;
								getchar();
								exit(-1);
							}
						} 
						else { //apsolutno adresiranje simbola
							if (prosirenja != 2) {
								if ((brojOperanda == 1 && (*it)->ime != "push")
										|| (brojOperanda == 2
												&& (*it)->ime == "xchg")) {
									std::cout << "Greska na liniji: "
											<< brLinije << endl;
									std::cout
											<< "Neprosirene instrukcijeee i neposredno adresiranje nije dozvoljeno za dst operand!"
											<< endl;
									getchar();
									exit(-1);
								}
							}

							if ((*iter).find_first_of("[]&*") != string::npos) {
								std::cout << "Greska na liniji: " << brLinije
										<< endl;
								std::cout << "Pogresno imenovanje operanda!"
										<< endl;
								getchar();
								exit(-1);
							}

							if (tabela.find(*iter) == tabela.end()) { //novi simbol, dodati u tabelu
								Simbol *lab = new Simbol();
								lab->setNaziv(*iter);
								lab->setLocal();
								lab->setDef(false);
								lab->setRbr(tabela.size() + 1);
								tabela[(*iter)] = lab;
								if (prosirenja != 2) 
									konstante.push_back(lab);
								
							} 
							else {
								if (prosirenja != 2) konstante.push_back(tabela[*iter]);
							}
							bytes += 1 + prosirenja;
							continue;

						}

						//uzeti argumente, proveriti tipove adresiranja, 
						//proveriti ima li labela, ako ima a ne postoje u tabeli
						//dodati kao nedefinisane, ako ih ima preko 2 greska, 
						//ako ih ima dve a nisu iz iste sekcije ili nije - izmedju salji gresku
					}

					adresa += bytes;
					break;
				}
			}
			if (ins == false) {
				greska = true;
				msg = "Neispravan kod!";
				break;
			}
		}
	}
	if (greska) {
		std::cout << "Greska na liniji: " << brLinije << endl;
		std::cout << "Poruka: " + msg << endl;
		getchar();
		exit(-1);
	}
	if (!ended) {
		std::cout << "Greska: Fajl se mora zavrsiti sa .end: " << endl;
		getchar();
		exit(-1);
	}
	return tabela;
}

void stampaj(string imeIzlaz, map<string, Simbol*> tabela,
		vector<Sekcija*> sekcije) {
	ofstream stream;
	stream.open(imeIzlaz.c_str(), ofstream::out);
	if (!stream.is_open()) {
		std::cout << "Nije otvoren fajl za izlaz!" << endl;
		getchar();
		exit(-1);
	}
	stream.flush();
	stream << "#TABELA SIMBOLA" << endl;
	stream << "#ime " << "sekcija " << "vrednost " << "vidljivost " << "r.b"
			<< endl;
	for (map<string, Simbol*>::iterator it = tabela.begin(); it != tabela.end();
			it++) {
		string name = it->first;
		Simbol* simb = tabela[name];
		Sekcija* sekcija = simb->getSek();
		stream << " " << it->first;
		stream << " " << sekcija->getNaziv();
		stream << " " << hex << simb->getVrednost();
		stream << " " << (simb->isGlobal() ? "G" : "L");
		stream << " " << dec << simb->getRbr();
		stream << endl;
	} // tabela simbola
	  //int row = 0;
	stream << endl;
	stream << "#TABELA RELOKACIJA" << endl;
	for (vector<Sekcija*>::iterator it = sekcije.begin(); it != sekcije.end();
			it++) {
		stream << "#rel" << (*it)->getNaziv() << endl;
		stream << "#offset " << "tip " << "vrednost " << endl;
		vector<Relokacija*> tabelaRelokacija = (*it)->getTabelaRelokacija();
		for (vector<Relokacija*>::iterator iter = tabelaRelokacija.begin();
				iter != tabelaRelokacija.end(); iter++) {
			stream << hex << (*iter)->getAdr() << " ";
			stream << (*iter)->getTip() << " ";
			stream << dec << (*iter)->getRbr() << endl;
			stream << endl;
		}
	}
	stream << "#MASINSKI KOD" << endl;
	for (vector<Sekcija*>::iterator it = sekcije.begin(); it != sekcije.end();
			it++) {
		if ((*it)->getNaziv() == ".bss") continue;
		stream << (*it)->getNaziv() << endl;
		int enter = 0;

		vector<string> masinskiKod = (*it)->kod;
		for (vector<string>::iterator iter = masinskiKod.begin();
				iter != masinskiKod.end(); iter++) {
			stream << *iter;
			if (enter == 15) {
				stream << endl;
				enter = 0;
			} else
				enter++;
		}
		stream << endl;
	}
	stream << "#end" << endl;
	stream.close();
}

bool ended;

int main(int argc, char* argv[]) {

	string imeUlaz = argv[1];
	string imeIzlaz = "output.txt";

	vector<Sekcija*> sekcije; //sve sekcije

	map<string, Simbol*> tabela;

	tabela = prviProlaz(imeUlaz, sekcije);
	//proveriti da li je svaki lokalni simbol definisan
	//ako nije, greska
	//ispisati u izlazni fajl tabelu simbola


	for (map<string, Simbol*>::iterator it = tabela.begin(); it != tabela.end();
			it++) {
		if (!it->second->isDefined()) {
			if (!it->second->isExtern()) {
				std::cout << "Greska: Nedefinisan simbol: " << it->first
						<< endl;
				getchar();
				exit(-1);
			}
		}

		if (it->second->isExtern()) {
			if (it->second->isDefined()) {
				std::cout << "Greska: Uvezeni simbol je definisan: "
						<< it->first << endl;
				getchar();
				exit(-1);
			}
		}
	}

	for (vector<Simbol*>::iterator iter = konstante.begin();
			iter != konstante.end(); ++iter) {
		if (!tabela[(*iter)->getNaziv()]->isConst()) {
			std::cout << "Greska: Simbol mora biti konstanta: "
					<< (*iter)->getNaziv() << endl;
			getchar();
			exit(-1);
		}
	}

	for (vector<Simbol*>::iterator iter = nisuKonstante.begin();
			iter != nisuKonstante.end(); ++iter) {
		if (tabela[(*iter)->getNaziv()]->isConst()) {
			std::cout << "Greska: Simbol ne sme biti konstanta: "
					<< (*iter)->getNaziv() << endl;
			getchar();
			exit(-1);
		}
	}
	// tabela simbola
	//int row = 0;
	//test
	//drugi prolaz 
	//trenutna sekcija je und

	DrugiProlaz* dp = new DrugiProlaz();
	dp->imeUlaz = imeUlaz;
	dp->prolaz(tabela, sekcije);

	stampaj(imeIzlaz, tabela, sekcije);

	return 0;
}

//makefile  nauciti i za odbranu
