#include "drugiProlaz.h"



const string registri[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" , "pc" "sp", "psw" };

bool DrugiProlaz::isRegister(string reg) {
	if (reg == "r0") return true;
	if (reg == "r1") return true;
	if (reg == "r2") return true;
	if (reg == "r3") return true;
	if (reg == "r4") return true;
	if (reg == "r5") return true;
	if (reg == "r6") return true;
	if (reg == "r7") return true;
	if (reg == "pc") return true;
	if (reg == "sp") return true;
	if (reg == "psw") return true;
	return false; //nije reg
} //moze bool ako mi ne treba 

bool DrugiProlaz::isRegisterLowHigh(string reg) {
	if (reg == "r0l") return true;
	if (reg == "r1l") return true;
	if (reg == "r2l") return true;
	if (reg == "r3l") return true;
	if (reg == "r4l") return true;
	if (reg == "r5l") return true;
	if (reg == "r6l") return true;
	if (reg == "r7l") return true;

	if (reg == "r0h") return true;
	if (reg == "r1h") return true;
	if (reg == "r2h") return true;
	if (reg == "r3h") return true;
	if (reg == "r4h") return true;
	if (reg == "r5h") return true;
	if (reg == "r6h") return true;
	if (reg == "r7h") return true;
	return false;
}

bool DrugiProlaz::isNumber(std::string s) {
	if (s.find("0x") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("0123456789ABCDEFabcdef") == string::npos)
			return true;
		else return false;
	}
	else if (s.find("0b") == 0) {
		s = s.substr(2);
		if (s.find_first_not_of("01") == string::npos)
			return true;
		else return false;
	}
	if (s.find_first_not_of("0123456789") == string::npos)
		return true;
	return false;
}

static vector<Instrukcija*> initialize() {
	Instrukcija* ins;
	vector<Instrukcija*> instrukcije;
	/* izmeniti oc i flegove i instrukcije*/
	//zapravo ne sme da se koristi imm za ove koje smestaju nesto u dst
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


	//jmp se prevodi u zav od tipa adresiranja, pcrel je add r7, pomeraj, od adrese lab na koju se skace se oduzme pc i dobije pom
	//ili kao mov r7, labela
	/*
	*/
	return instrukcije;
}

static vector<Instrukcija*> instrukcije = initialize();


//1 regdir
//0 imm
//2 regind
//3 regindpom
//5 mem
int DrugiProlaz::tipAdresiranja(string line) {
	pcrel = false;
	if (isRegister(line) || isRegisterLowHigh(line)) {
		return 1;
	}
	if (isNumber(line)) { //neposredno
		return 0;
	}
	if ((line).find_first_of("[") != string::npos) { //regind
		if (line.find("[") == 0)	//nopom
			return 2;
					//pom
		else {
			if (line.find("r7") == 0 || line.find("pc") == 0)
				pcrel = true;
			return 3;
		}
	}
	if ((line).find_first_of("&") == 0) { //imm
		return 0;
	}
	if ((line).find_first_of("*") == 0) {	//mem
		return 5;
	}
	if ((line).find_first_of("$") == 0) { //pcrel je regindpom
		pcrel = true;
		return 3;
	}
	if ((line).find_first_not_of("0123456789") != string::npos) { //simbol, mem
		return 5;
	}
	
	return -1;
}


int DrugiProlaz::duzinaInstrukcije(string line) {
	pcrel = false; pcrel1 = false; pcrel2 = false;
	int len = 1;
	string name = "";
	//deklaracije
	int prosirenja = 0;
	
	//instrukcije
	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if (line.find((*it)->ime) == 0) {
			inst = *it;
			name = inst->ime;
			line = line.substr(name.size());
			if (line[0] == 'b') prosirenja = 1;
			else if (line[0] == 'w') prosirenja = 2;
			line = line.substr(1);
			line = trimSpace(line);
			break;
		}
	}

	if (inst->brParam == 0) {
		adresiranjeSrc = ""; src = "";
		adresiranjeDst = ""; dst = "";
		return len;
	}
	else 
	if (inst->brParam == 1) {
		len++;
		for (vector<Simbol*>::iterator iter = konstante.begin();
					iter != konstante.end(); ++iter) {
					if ((*iter)->getNaziv() == line) {
						ostringstream convert;
						convert << (*iter)->getVrednost();

						line = convert.str();
					}
				}
		/*
			if(name == "POP"){
				src = ""; adresiranjeSrc = "00";
				int tipadr = tipAdresiranja(line);
				if (tipadr == 0) {
					adresiranjeDst = "00";
				}
				else if (tipadr == 1) {
					adresiranjeDst = "01"; len = 2;
				}
				else if (tipadr == 2) {
					if (line.find_first_of("&*") == 0) line = line.substr(1);
					adresiranjeDst = "10";
				}
				else if (tipadr == 3){
					regDst = line.substr(0, 2);
					line = line.substr(3, line.length() - 4);
					adresiranjeDst = 11;
				}
				else {
					cout << "Greska, pogresno adresiranje " << line << endl;
					getchar();
					exit(-1);
				}

				dst = line; return len;
			}  // ovo ce se brishe 
			else if (name == "JMP") {
				pcrel = true;
				if ((line).find_first_of("$") == 0) {
					(line) = (line).substr(1);
					for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
						if ((*it)->ime == "ADD") {
							inst = *it;
							name = "ADD";
							break;
						}
					}
					dst = "R7"; adresiranjeDst = "11"; //regdir
					if (line.find_first_not_of("0123456789") == string::npos) {//neposredno
						src = line; adresiranjeSrc = "00"; return 4;
					} 
					else if (tabelaSimbola.find(line) == tabelaSimbola.end()) {
						cout << "Sme se skakati samo na labelu!" << endl;
						getchar();
						exit(1);
					}
					src = line; adresiranjeSrc = "10"; //memorijsko direktno
					return 4;
				}
				else {
					for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
						if ((*it)->ime == "MOV") {
							inst = *it;
							name = (*it)->ime;
							break;
						}
					}
					dst = "R7"; adresiranjeDst = "01"; //regdir
					if (tabelaSimbola.find(line) == tabelaSimbola.end()) {
						greska = true;
						msg = "Sme se skakati samo na labelu!";
						return -1;
					}
					src = line; adresiranjeSrc = "10"; //memorijsko direktno
					return 4;
				}
			}
			else { //prethodno je zabranjeno bilo koje drugo adresiranje osim memdir za call
				dst = "000"; adresiranjeDst = "00";

				int tipadr = tipAdresiranja(line);
				if (name == "CALL") {
					if (tipadr != 2) {
							cout << "Instrukcija call sme samo uz memorijsko direktno adresiranje  " << line << endl;
							getchar();
							exit(1);
						}
					
					if (line.find_first_of("&*") == 0) { //line = line.substr(1); 
						cout << "Instrukcija call sme samo uz memorijsko direktno adresiranje  " << line << endl;
						getchar();
						exit(1);
					}
				}
				if (tipadr == 0) {
					adresiranjeSrc = "00";  src = line; return len;
				}
				else if (tipadr == 1) {
					adresiranjeSrc = "01"; len = 2; src = line; return len;
				}
				else if (tipadr == 2) {
					if (line.find_first_of("&*") == 0) { line = line.substr(1); 
					}
					adresiranjeSrc = "10"; src = line; return len;
				}
				else if (tipadr == 3){
					regSrc = line.substr(0, 2);
					line = line.substr(3, line.length() - 4);
					adresiranjeSrc = "11"; src = line; return len;
				}
				else {
					cout << "Pogresno adresiranje " << line << endl;
					getchar();
					exit(-1);
				}
			}
			*/
			if (name == "push") {
				dst = ""; adresiranjeDst = "000";
				int tipadr = tipAdresiranja(line);
				if (pcrel == true) {
					pcrel = false;
					pcrel2 = true;
				}
				if (tipadr == 0) {
					if (line.find_first_of("&") == 0) line = line.substr(1);
					adresiranjeSrc = "000";
					src = line;
					len += prosirenja;
				}
				else if (tipadr == 1) {
					adresiranjeSrc = "001";
					src = line;
					//len += 1;
				}
				else if (tipadr == 2) {
					line = line.substr(1, line.length() - 2);
					src = line;
					adresiranjeSrc = "010";
					//len += 1;
				}
				else if (tipadr == 3) {
					
					if ((line).find_first_of("$") == 0) {
						line = line.substr(1);
						regSrc = "r7";
						src = line;

						if (prosirenja == 1) adresiranjeSrc = "011";
						else adresiranjeSrc = "100";
						len += prosirenja;
					}
					else {
						regSrc = line.substr(0, line.find("["));
						line = line.substr(line.find("["));
						line = line.substr(1);
						line = line.substr(0, line.length() - 1);
						src = line;

						if (prosirenja == 1) adresiranjeSrc = "011";
						else adresiranjeSrc = "100";

						len += prosirenja;
					}
					
					
				}
				else if (tipadr == 5) {
					if ((line).find_first_of("*") == 0)
						line = line.substr(1);
					adresiranjeSrc = "101";
					src = line;
					len += prosirenja;
				}
				else {
					cout << "Greska, pogresno adresiranje " << line << endl;
					getchar();
					exit(-1);
				}
			}
			else {
				src = ""; adresiranjeSrc = "000";
				int tipadr = tipAdresiranja(line);
		//		if (name == "jmp" || name == "jeq" || name == "jne" || name == "jgt") pcrel = true;
				if (pcrel == true) {
					pcrel = false;
					pcrel1 = true;
				}
				if (tipadr == 0) {
					if (line.find_first_of("&") == 0) line = line.substr(1);
					adresiranjeDst = "000";
					dst = line;
					len += prosirenja;
				}
				else if (tipadr == 1) {
					adresiranjeDst = "001";
					dst = line;
					//len += 1;
				}
				else if (tipadr == 2) {
					line = line.substr(1, line.length() - 2);
					dst = line;
					adresiranjeDst = "010";
					//len += 1;
				}
				else if (tipadr == 3) {

					if ((line).find_first_of("$") == 0) {
						line = line.substr(1);
						regDst = "r7";
						dst = line;

						if (prosirenja == 1) adresiranjeDst = "011";
						else adresiranjeDst = "100";
						len += prosirenja;
					}
					else {
						regDst = line.substr(0, line.find("["));
						line = line.substr(line.find("["));
						line = line.substr(1);
						line = line.substr(0, line.length() - 1);
						dst = line;

						if (prosirenja == 1) adresiranjeDst = "011";
						else adresiranjeDst = "100";

						len += prosirenja;
					}


				}
				else if (tipadr == 5) {
					if ((line).find_first_of("*") == 0)
						line = line.substr(1);
					adresiranjeDst = "101";
					dst = line;
					len += prosirenja;
				}
				else {
					cout << "Greska, pogresno adresiranje " << line << endl;
					getchar();
					exit(-1);
				}
			}
	}
	else {
		vector<string> tokeni = tokenize(line, ',');
		//int brTokena = tokeni.size(); //broj parametara

		int brojOperanda = 0;

		for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
			len++;
			for (vector<Simbol*>::iterator iterat = konstante.begin();
					iterat != konstante.end(); ++iterat) {
								if ((*iterat)->getNaziv() == (*iter)) {
									ostringstream convert;
									convert << (*iterat)->getVrednost();

									(*iter) = convert.str();
								}
							}
			string cachedIter = (*iter);
			int num = 0; //provera ispravnosti adresiranja
			brojOperanda++; (*iter) = trimSpace(*iter);
			int tipadr = tipAdresiranja(*iter);
			if (pcrel == true) {
				pcrel = false;
				if (brojOperanda == 1) pcrel2 = true;
				else pcrel2 = true;
			}
			if (tipadr == 0) {

				if ((*iter).find_first_of("&") == 0) (*iter) = (*iter).substr(1);
				len += prosirenja;
				if (brojOperanda == 1) {
					adresiranjeDst = "000";
					dst = (*iter);
				}
				else {
					adresiranjeSrc = "000";
					src = (*iter);
				}
			}
			else if (tipadr == 1) {
				//len += 1;
				if (brojOperanda == 1) {
					adresiranjeDst = "001";
					dst = (*iter);
				}
				else {
					adresiranjeSrc = "001";
					src = (*iter);
				}
			}
			else if (tipadr == 2) {
				//len += 1;
				(*iter) = (*iter).substr(1, (*iter).length() - 2);

				if (brojOperanda == 1) {
					adresiranjeDst = "010";
					dst = (*iter);
				}
				else {
					adresiranjeSrc = "010";
					src = (*iter);
				}
			}
			else if (tipadr == 3) {
			len += prosirenja;

				if (((*iter)).find_first_of("$") == 0) {
					(*iter) = (*iter).substr(1);

					if (brojOperanda == 2) {
						regSrc = "r7";
						src = (*iter);

						if (prosirenja == 1) adresiranjeSrc = "011";
						else adresiranjeSrc = "100";
					}
					else {
						regDst = "r7";
						dst = (*iter);

						if (prosirenja == 1) adresiranjeDst = "011";
						else adresiranjeDst = "100";
					}
				}
				else {
					if (brojOperanda == 2) {
						regSrc = (*iter).substr(0, (*iter).find("["));
						(*iter) = (*iter).substr((*iter).find("["));
						(*iter) = (*iter).substr(1);
						(*iter) = (*iter).substr(0, (*iter).length() - 1);
						src = (*iter);

						if (prosirenja == 1) adresiranjeSrc = "011";
						else adresiranjeSrc = "100";
					}
					else {
						regDst = (*iter).substr(0, (*iter).find("["));
						(*iter) = (*iter).substr((*iter).find("["));
						(*iter) = (*iter).substr(1);
						(*iter) = (*iter).substr(0, (*iter).length() - 1);
						dst = (*iter);

						if (prosirenja == 1) adresiranjeDst = "011";
						else adresiranjeDst = "100";
					}
				}


			}
			else {
				len += prosirenja;
				if ((*iter).find_first_of("*") == 0)
					(*iter) = (*iter).substr(1);
				if (brojOperanda == 1) {
					adresiranjeDst = "101";
					dst = (*iter);
				}
				else {
					adresiranjeSrc = "101";
					src = (*iter);
				}
			}
	}
	}

	return len;
}


int DrugiProlaz::trebaRelokacija() {
	//if (pcrel) return 1;

	if (dst != "" && !isRegister(dst) && !isRegisterLowHigh(dst) && !isNumber(dst) && src != "" && !isRegister(src) && !isRegisterLowHigh(src) && !isNumber(src)) {
		bool dest = false;
		if (tabelaSimbola.find(dst) != tabelaSimbola.end()) {
			Simbol* simb = tabelaSimbola[dst];
			if (simb->getSek()->getNaziv() != sekcija->getNaziv()) {
				dest = true;
			}
		}
		if (tabelaSimbola.find(src) != tabelaSimbola.end()) {
			Simbol* simb = tabelaSimbola[src];
			if (simb->getSek()->getNaziv() != sekcija->getNaziv()) {
				if (dest) return 3;
			}
		}
	}

	if (dst != "" && !isRegister(dst) && !isRegisterLowHigh(dst) && !isNumber(dst)) {
		if (tabelaSimbola.find(dst) != tabelaSimbola.end()) {
			Simbol* simb = tabelaSimbola[dst];
			if (simb->getSek()->getNaziv() != sekcija->getNaziv()) {
				return 1;
			}
		}
	}
	if (src != "" && !isRegister(src) && !isRegisterLowHigh(src) && !isNumber(src)) {
		if (tabelaSimbola.find(src) != tabelaSimbola.end()) {
			Simbol* simb = tabelaSimbola[src];
			if (simb->getSek()->getNaziv() != sekcija->getNaziv()) {
				return 2;
			}
		}
	}
	return 0;
}

void DrugiProlaz::relokacija(long adrr, bool pcrel, int num) {
	string tip = "R_386_16";
	if (pcrel) tip = "R_386_PC16";
	//this->pcrel = false;
	string adresaZaRelokaciju = longToHex(adrr);
	Relokacija* relokacija = new Relokacija();
	relokacija->setAdr(adresaZaRelokaciju);
	relokacija->setTip(tip);
	relokacija->setRbr(num);
	sekcija->tabelaRelokacija.push_back(relokacija);
}


void DrugiProlaz::obradiInstrukciju(string line) {
	if (line == "") return;
	vector<string> pom;
	int num;

	int prosirenja = 0;
	string imeInstr;
	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if (line.find((*it)->ime) == 0) {
			imeInstr = (*it)->ime;
			string st = line.substr((*it)->ime.size());
			if (st[0] == 'b') prosirenja = 1;
			else if (st[0] == 'w') prosirenja = 2;
			break;
		}
	}

		int len = duzinaInstrukcije(line);
		int r = trebaRelokacija();		// 1 treba za dst, 2 treba za scr, 0 ne treba
		if (r != 0) {
			if (r == 3) {
				string naziv1;
				string naziv2;
				naziv1 = dst;
				naziv2 = src;
				Simbol* simb1 = tabelaSimbola[naziv1];
				Simbol* simb2 = tabelaSimbola[naziv2];
				int num1, num2;
				if (simb1->isGlobal())
					num1 = simb1->getRbr();
				else {
					Sekcija* sek2 = simb1->getSek();
					string nazsek = sek2->getNaziv();
					Simbol* seksimb = tabelaSimbola[nazsek];
					num1 = seksimb->getRbr();
				}
				if (simb2->isGlobal())
					num2 = simb2->getRbr();
				else {
					Sekcija* sek2 = simb2->getSek();
					string nazsek = sek2->getNaziv();
					Simbol* seksimb = tabelaSimbola[nazsek];
					num1 = seksimb->getRbr();
				}
				int pomeraj1 = 1, pomeraj2 = 1;

				
				pomeraj2 += 1;
				
				int dopuna = prosirenja;
				pomeraj1 = pomeraj2 + dopuna + 1;
				
				relokacija(adresa + pomeraj2, pcrel2, num2);
				relokacija(adresa + pomeraj1, pcrel1, num1);
			}
			else {
				string naziv;
				if (r == 1) naziv = dst;
				else naziv = src;
				Simbol* simb = tabelaSimbola[naziv];
				if (simb->isGlobal())
					num = simb->getRbr();
				else {
					Sekcija* sek2 = simb->getSek();
					string nazsek = sek2->getNaziv();
					Simbol* seksimb = tabelaSimbola[nazsek];
					num = seksimb->getRbr();
				}
				int pomeraj = 1;

				if (r == 1) { //dst
					pomeraj += 1;
					pcrel2 = false;
				}
				else { //src
					pcrel1 = false;
					if (imeInstr == "push") pomeraj += 1;
					else {
						int dopuna = prosirenja;
						if (adresiranjeDst == "001" || adresiranjeDst == "010") dopuna = 0;
						pomeraj = pomeraj + 1 + dopuna + 1;
					}
				}
				if (r == 1) relokacija(adresa + pomeraj, pcrel1, num);
				else relokacija(adresa + pomeraj, pcrel2, num);
			}
		}
		else {
			pcrel2 = pcrel1 = false;
		}

		adresa += len;
} //srediti

/*
void DrugiProlaz::instruction(string rest) {
	bool ins = false;
	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		//u zav od tipa adresiranja adresa += 2 ili 4 bajta
		//ako nema arg koji nije regdir onda 2 b inace je 4
		if (rest.find((*it)->ime) == 0) {
			ins = true;
			int bytes = 2;
			rest.substr((*it)->ime.length());
			rest = trimSpace(rest);
			vector<string> tokeni = tokenize(rest, ',');
			int brTokena = tokeni.size(); //broj parametara
			int brojOperanda = 0;
			if (brTokena != (*it)->brParam) {
				greska = true;
				msg = "Pogresan broj parametara instrukcije!";
				break;
			}
			int prosirenja = 0; //provera koliko puta je potrebno prosirenje
			for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
				string cachedIter = (*iter);
				int num = 0; //provera ispravnosti adresiranja
				brojOperanda++;
				(*iter) = trimSpace(*iter);
				if (!isRegister(*iter)) {
					bytes = 4; prosirenja++;
				}
				if ((*iter).find_first_of("[]") != string::npos) { //regindpom
					(*iter) = (*iter).substr(3, (*iter).length() - 4);
					num++;
				}
				if ((*iter).find_first_of("&") == 0) {
					(*iter) = (*iter).substr(1);
					if ((*iter).find_first_not_of("0123456789") == string::npos) {
						greska = true;
						msg = "Pogresno adresiranje operanada!";
						break;
					}
					num++;
				}
				if ((*iter).find_first_of("*") == 0) {
					(*iter) = (*iter).substr(1);
					if ((*iter).find_first_not_of("0123456789") != string::npos) {
						greska = true;
						msg = "Pogresno adresiranje operanada!";
						break;
					}
					num++;
				}
				if ((*iter).find_first_not_of("0123456789") != string::npos) { //simbol
					if ((*iter).find_first_of("[]&*") != string::npos) {
						greska = true;
						msg = "Greska u imenovanju operanda!";
						break;
					}
					string name = (*iter);
					if (tabelaSimbola.find(name) == tabelaSimbola.end()) {

						Sekcija* sek = new Sekcija();
						sek->setNaziv("UND");
						Simbol* lab = new Simbol();
						lab->setNaziv(name);
						lab->setSek(sek);
						lab->setLocal();
						lab->setDef(false);
						tabelaSimbola[name] = lab;

					}
				}
				if (cachedIter.find_first_not_of("0123456789") == string::npos) { //neposredno
					if (brojOperanda == 1 && brTokena == 2 || (*it)->ime == "POP") {
						greska = true;
						msg = "Nije dozvoljeno neposredno adresiranje kod dst operanda!";
						break;
					}

				}
				if (num > 1) {
					greska = true;
					msg = "Greska u adresiranju!";
					break;
				}
				//uzeti argumente, proveriti tipove adresiranja, 
				//proveriti ima li labela, ako ima a ne postoje u tabeli
				//dodati kao nedefinisane, ako ih ima preko 2 greska, 
				//ako ih ima dve a nisu iz iste sekcije ili nije - izmedju salji gresku
			}
			if (prosirenja > 1) {
				msg = "Nedozvoljen spoj adresiranja";
				greska = true;
				break;
			}
			adresa += bytes;
			break;
		}
	}
}
*/
void DrugiProlaz::prolaz(map<string, Simbol*> tabela, vector<Sekcija*>& sek) {
	tabelaSimbola = tabela;
			ifstream myfile; //labele definicije instrukcije
			myfile.open(imeUlaz.c_str());
			if (!myfile.is_open()) {
				cout << "Nije otvoren ulazni fajl";
				getchar();
				exit(-1);
			}
			myfile.seekg(0, ios::beg);


	for (map<string, Simbol*>::iterator it = tabela.begin(); it != tabela.end();
		it++) {
		if (it->second->isConst()) konstante.push_back(it->second);
	}

	//string rest;
	string line;
	bool greska = false;
	adresa = 0;
	for (unsigned i = 0; i < sek.size(); i++) {
		sekcije.push_back(sek[i]); 
	}
	int brLinije = 0;
	while (getline(myfile, line)) {
		brLinije++;
		src = ""; dst = ""; regSrc = ""; regDst = ""; opcode = "";


		line = trimComm(line);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		line = trimSpace(line);
		//rest = line;

		if (line.empty()) continue;
		if (line.find(".global") == 0) {
			continue;
		} else 
			if (line.find(".extern") == 0) {
			continue;
		}
		else
		if (line.find_first_of(":")) {  // there is a label
			size_t pos = line.find_first_of(":");
			line = line.substr(pos + 1);
			line = trimSpace(line);
			while (line.empty()) { //za slucaj da labela stoji u praznom redu i da ispod postoji praznih redova
				brLinije++;
				getline(myfile, line);
				line = trimComm(line);
				line = trimSpace(line);
			} 
			if (line.find_first_of(":") != string::npos) {
				cout << "Nije dozvoljena labela za labelom!" << endl;
				getchar();
				exit(1);
			}// dalje sekcije
			  //obradi instrukciju
			//obradiInstrukciju(line);
			//generisiInstr(line, adresa);
		}//ne treba
		if (line.find(".text") == 0 || line.find(".data") == 0
			|| line.find(".bss") == 0) { //sekcija
			string naziv;
			if (line.find(".text") != string::npos) {
				naziv = ".text";
			}
			else if (line.find(".data") != string::npos) {
				naziv = ".data";
			}
			else if (line.find(".bss") != string::npos) {
				naziv = ".bss";
			}
			line = "";
			for (vector<Sekcija*>::iterator it = sekcije.begin(); it != sekcije.end(); ++it) {
				if ((*it)->getNaziv() == naziv) {
					sekcija = (*it);
					break;
				}
			}

			//string enter;
			//putCode(enter);
			adresa = 0;
			continue;
		}
		if (line.find(".section") == 0) { //sekcija
			string naziv;
			line = trimSpace(line.substr(8));
			vector<string> tokens = tokenize(line, ',');
			vector<string>::iterator it = tokens.begin();
			(*it) = trimSpace(*it);
			naziv = (*it);

			line = "";
			for (vector<Sekcija*>::iterator it = sekcije.begin(); it != sekcije.end(); ++it) {
				if ((*it)->getNaziv() == naziv) {
					sekcija = (*it);
					break;
				}
			}

			//string enter;
			//putCode(enter);
			adresa = 0;
			continue;
		}
		if (line.find(".end") == 0) {
			break;
		}
		if (line.find(".equ") == 0) {
			continue;
		}
		if (line.find(".byte") == 0 || line.find(".word") == 0) {
			int bytes;

			if (line.find(".word") == 0) {
				bytes = 2;
				line = line.substr(line.find(".word") + 5);

			}
			else if (line.find(".byte") == 0) {
				bytes = 1;
				line = line.substr(line.find(".byte") + 5);
			}

			vector<string> tokens = tokenize(line, ',');
			int delovi = 0;
			int cachedAdresa = adresa;
			for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
				delovi++;
				(*it) = trimSpace(*it);
				Parser* parser = new Parser(*it, &tabela);

				parser->setKonstante(konstante);

				parser->checkSymbols(sekcija);

				if (parser->getGreska()) {
					cout << parser->getMsg() << endl;
					getchar();
					exit(1);

				}
				int num;
				if (parser->trebaRel()) {
					Simbol* simbolZaRel = parser->getSimbolZaRelokaciju();
					if (simbolZaRel->isGlobal())
						num = simbolZaRel->getRbr();
					else
						num = tabelaSimbola[simbolZaRel->getSek()->getNaziv()]->getRbr();
					relokacija(cachedAdresa, false, num);
				}
				
					string izrazz = parser->getIzraz();
					int vrednost = Parser::evaluate(izrazz);
					if (bytes == 1) dotChar(vrednost); //preimenovati u dotByte mozda
					else if (bytes == 2) dotWord(vrednost);

					cachedAdresa += bytes;
			}


			adresa +=  delovi * bytes;
			continue;
			//zapisi
		}
		else if (line.find(".align") == 0 || line.find(".skip") == 0) {
			bool align = false; 
			if (line.find(".align") == 0) {
				line = line.substr(line.find(".align") + 6);
				align = true;
			}
			else if (line.find(".skip") == 0) {
				line = line.substr(line.find(".skip") + 5);
				align = false;
			}

			line = trimSpace(line);
			int value = Parser::evaluate(line);
			long cachedAddr = adresa;
			if (align) {
				if (adresa == 0) continue;
				adresa = (adresa - (adresa % value)) + value;
			}
			else {
				adresa += value;
			}

			cachedAddr = adresa - cachedAddr; //pomeraj
			for (int i = 0; i < cachedAddr; i++) {
				sekcija->addCode("00"); //preskociti pomeraj * bajt
			}
			continue;
		}
		else {
			obradiInstrukciju(line);
			generisiInstr(line, adresa);
		}
	}
	if (greska) {
		cout << msg << endl;
		getchar();
		exit(-1);
	}
}

void DrugiProlaz::generisiInstr(string line, int adr) {
	opcode = "";
	int prosirenja = 0;
	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if (line.find((*it)->ime) == 0) {
			string st = line.substr((*it)->ime.size());
			if (st[0] == 'b') prosirenja = 1;
			else if (st[0] == 'w') prosirenja = 2;
			break;
		}
	}

	line = trimSpace(line);

	if (line.find("add") == 0)
		add(line, adr, prosirenja);
	else if (line.find("sub") == 0)
		sub(line, adr, prosirenja);
	else if (line.find("mul") == 0)
		mul(line, adr, prosirenja);
	else if (line.find("div") == 0)
		div(line, adr, prosirenja);
	else if (line.find("cmp") == 0)
		cmp(line, adr, prosirenja);
	else if (line.find("and") == 0)
		andd(line, adr, prosirenja);
	else if (line.find("or") == 0)
		orr(line, adr, prosirenja);
	else if (line.find("not") == 0)
		nott(line, adr, prosirenja);
	else if (line.find("test") == 0)
		test(line, adr, prosirenja);
	else if (line.find("push") == 0)
		push(line, adr, prosirenja);
	else if (line.find("pop") == 0)
		pop(line, adr, prosirenja);
	else if (line.find("call") == 0)
		call(line, adr, prosirenja);
	else if (line.find("iret") == 0)
		iret(line, adr, prosirenja);
	else if (line.find("mov") == 0)
		mov(line, adr, prosirenja);
	else if (line.find("shl") == 0)
		shl(line, adr, prosirenja);
	else if (line.find("shr") == 0)
		shr(line, adr, prosirenja);
	else if (line.find("ret") == 0)
		ret(line, adr, prosirenja);
	else if (line.find("jmp") == 0)
		jmp(line, adr, prosirenja);
	else if (line.find("jeq") == 0)
		jeq(line, adr, prosirenja);
	else if (line.find("jne") == 0)
		jne(line, adr, prosirenja);
	else if (line.find("jgt") == 0)
		jgt(line, adr, prosirenja);
	else if (line.find("halt") == 0)
		halt(line, adr, prosirenja);
	else if (line.find("xchg") == 0)
		xchg(line, adr, prosirenja);
	else if (line.find("int") == 0)
		intt(line, adr, prosirenja);
	else if (line.find("xor") == 0)
		xorr(line, adr, prosirenja);

}

void DrugiProlaz::dotChar(int line) {
	string hexa = toHex(line);
	int lengthBin = hexa.length();
	if (lengthBin > 2 && line > 0) {
		cout << "Prevelika vrednost za .char " << line << endl;
		getchar();
		exit(-1);
	}
	else if (lengthBin > 2 && line < 0) {
		hexa = hexa.substr(hexa.length() - 2, 2);
	}
	else
		if (lengthBin < 2) {
			hexa = "0" + hexa;
		}

	sekcija->addCode(hexa);

}
void DrugiProlaz::dotWord(int line) {
	string hexa = toHex(line);

	int lengthBin = hexa.length();
	if (lengthBin > 4 && line > 0) {
		cout << "Prevelika vrednost za .word " << line << endl;
		getchar();
		exit(-1);
	}
	else if (lengthBin > 4 && line < 0) {
		hexa = hexa.substr(hexa.length() - 4, 4);
	}
	else
		if (lengthBin < 4) {
			for (int i = 0; i < 4 - lengthBin; i++)
				hexa = "0" + hexa;
		}
	string little = hexa.substr(2, 2);
	string big = hexa.substr(0, 2);
	sekcija->addCode(little);
	sekcija->addCode(big);
}
void DrugiProlaz::dotLong(int line) {
	string hexa = toHex(line);
	int lengthBin = hexa.length();
	if (lengthBin > 8 && line > 0) {
		cout << "Prevelika vrednost za .long " << line << endl;
		getchar();
		exit(-1);
	}
	else if (lengthBin > 2 && line < 0) {
		hexa = hexa.substr(hexa.length() - 8, 8);
	}
	else
		if (lengthBin < 8) {
			for (int i = 0; i < 8 - lengthBin; i++)
				hexa = "0" + hexa;
		}
	while (hexa.length() > 0) {
		string little = hexa.substr(hexa.length() - 2, 2);
		string big = hexa.substr(hexa.length() - 4, 2);
		sekcija->addCode(little);
		sekcija->addCode(big);
		hexa = hexa.substr(0, hexa.length() - 4);
	}
}


void DrugiProlaz::ubaciHex(string hexa) {
	while (hexa.length() >= 4) {
		string little = hexa.substr(hexa.length() - 2, 2);
		string big = hexa.substr(hexa.length() - 4, 2);
		sekcija->addCode(little);
		sekcija->addCode(big);
		hexa = hexa.substr(0, hexa.length() - 4);
	}
	if (hexa.length() == 2){
	sekcija->addCode(hexa);
	}
}

void DrugiProlaz::extendInstruction(string simbol, int prosirenja, bool pcrel, int oduzimanje) {
	if (!pcrel) {
		if (prosirenja == 2) {
			if (!isNumber(simbol)) { //simbol
				Simbol* sim = tabelaSimbola[simbol];
				if (sim->isGlobal()) {
					ubaciHex("0000");
				}
				else {
					int vrednost = sim->getVrednost();
					dotWord(vrednost); //2B, treba vrednost pa koristim
				}
			}
			else dotWord(atoi(simbol.c_str()));
		}
		else {
			if (!isNumber(simbol)) { //simbol
				Simbol* sim = tabelaSimbola[simbol];
				if (sim->isGlobal()) {
					ubaciHex("00");
				}
				else {
					int vrednost = sim->getVrednost();
					dotChar(vrednost); //2B, treba vrednost pa koristim
				}
			}
			else dotChar(atoi(simbol.c_str()));
		}
	}
	else {
		if (prosirenja == 2) {
			if (!isNumber(simbol)) { //simbol
				Simbol* sim = tabelaSimbola[simbol];
				if (sim->isGlobal()) {
					dotWord(0 - oduzimanje);
				}
				else {
					int vrednost = sim->getVrednost();
					dotWord(vrednost - oduzimanje); //2B, treba vrednost pa koristim
				}
			}
			else dotWord(atoi(simbol.c_str()) - oduzimanje);
		}
		else {
			if (!isNumber(simbol)) { //simbol
				Simbol* sim = tabelaSimbola[simbol];
				if (sim->isGlobal()) {
					dotChar(0 - oduzimanje);
				}
				else {
					int vrednost = sim->getVrednost();
					dotChar(vrednost - oduzimanje); //2B, treba vrednost pa koristim
				}
			}
			else dotChar(atoi(simbol.c_str()) - oduzimanje);
		}
	}
}


void DrugiProlaz::halt(string line, long adr, int prosirenja) {
	string opcod = "00001"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	
	kodInstrukcije = opcod + size + "00";
	
	sekcija->addCode(binToHex(kodInstrukcije));

}
void DrugiProlaz::xchg(string line, long adr, int prosirenja) {
	string opcod = "00010"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "xchg") opcode = (*it)->opcode;
		line = line.substr(4);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")

				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0; //vracanje pc-a
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);

		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::intt(string line, long adr, int prosirenja) {
	string opcod = "00011"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "int") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	

			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);


}


void DrugiProlaz::mov(string line, long adr, int prosirenja) {
	string opcod = "00100"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "mov") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);

		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));
			
			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}

void DrugiProlaz::add(string line, long adr, int prosirenja) {
	string opcod = "00101"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "add") opcode = (*it)->opcode; 
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
				}
				else if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100") 
				regDstCode = registerCode(regDst);
			

			if (adresiranjeDst == "") adresiranjeDst = "000"; 
			
			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);

		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::sub(string line, long adr, int prosirenja) {
	string opcod = "00110"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "sub") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);

		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}
			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::mul(string line, long adr, int prosirenja) {
	string opcod = "00111"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "mul") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::div(string line, long adr, int prosirenja) {
	string opcod = "01000"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "div") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::cmp(string line, long adr, int prosirenja) {
	string opcod = "01001"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "cmp") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}


void DrugiProlaz::nott(string line, long adr, int prosirenja) {
	string opcod = "01010"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "not") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	string lohi = "0";

	if (adresiranjeDst == "001") {
		if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
			dst = dst.substr(0, dst.length() - 1);
			lohi = "1";
			if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

		}
	}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);

		

	}


void DrugiProlaz::andd(string line, long adr, int prosirenja) {
	string opcod = "01011"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "and") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::orr(string line, long adr, int prosirenja) {
	string opcod = "01100"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "or") opcode = (*it)->opcode;
		line = line.substr(2);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::xorr(string line, long adr, int prosirenja) {
	string opcod = "01101"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "xor") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::test(string line, long adr, int prosirenja) {
	string opcod = "01110"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "test") opcode = (*it)->opcode;
		line = line.substr(4);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}


void DrugiProlaz::shl(string line, long adr, int prosirenja) {
	string opcod = "01111"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "shl") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}
void DrugiProlaz::shr(string line, long adr, int prosirenja) {
	string opcod = "10000"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "shr") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	vector<string> tokeni = tokenize(line, ',');
	int brojOperanda = 0;
	for (vector<string>::iterator iter = tokeni.begin(); iter != tokeni.end(); ++iter) {
		brojOperanda++;
		if (brojOperanda == 1) {
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			int oduzimanje = 0;
			if (pcrel1) {
				int drugiop;
				if (adresiranjeSrc == "001" || adresiranjeSrc == "010") drugiop = 1;
				else drugiop = 1 + prosirenja;
				oduzimanje = prosirenja + drugiop;
			}
			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, oduzimanje);
		}
		else {
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);

		}

	}

}

void DrugiProlaz::push(string line, long adr, int prosirenja) {
	string opcod = "10001"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op2descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "push") opcode = (*it)->opcode;
		line = line.substr(4);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	
			string lohi = "0";
			if (adresiranjeSrc == "001") {
				if (src[src.length() - 1] == 'h' && !isRegister(src)) {
					src = src.substr(0, src.length() - 1);
					lohi = "1";
				}
				if (src[src.length() - 1] == 'l') src = src.substr(0, src.length() - 1);
			}

			if (isRegister(src)) regSrc = src;

			string regSrcCode = "0000";
			if (adresiranjeSrc == "001" || adresiranjeSrc == "010" || adresiranjeSrc == "011" || adresiranjeSrc == "100")
				regSrcCode = registerCode(regSrc);


			if (adresiranjeSrc == "") adresiranjeSrc = "000";

			op2descr = adresiranjeSrc + regSrcCode + lohi;
			sekcija->addCode(binToHex(op2descr));

			if (!isRegister(src) && !isRegisterLowHigh(src)) extendInstruction(src, prosirenja, pcrel2, prosirenja);


}
void DrugiProlaz::pop(string line, long adr, int prosirenja) {
	string opcod = "10010"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "pop") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);


}

void DrugiProlaz::jmp(string line, long adr, int prosirenja) {
	string opcod = "10011"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "jmp") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	
	sekcija->addCode(binToHex(kodInstrukcije));

			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);

	}
void DrugiProlaz::jeq(string line, long adr, int prosirenja) {
	string opcod = "10100"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "jeq") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);

}
void DrugiProlaz::jne(string line, long adr, int prosirenja) {
	string opcod = "10101"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "jne") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));

	
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);

		

}
void DrugiProlaz::jgt(string line, long adr, int prosirenja) {
	string opcod = "10110"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "jgt") opcode = (*it)->opcode;
		line = line.substr(3);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));
			string lohi = "0";
			if (adresiranjeDst == "001") {
				if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
					dst = dst.substr(0, dst.length() - 1);
					lohi = "1";
					if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

				}
			}

			if (isRegister(dst)) regDst = dst;

			string regDstCode = "0000";
			if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
				regDstCode = registerCode(regDst);


			if (adresiranjeDst == "") adresiranjeDst = "000";

			op1descr = adresiranjeDst + regDstCode + lohi;
			sekcija->addCode(binToHex(op1descr));

			if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);

		
}

void DrugiProlaz::call(string line, long adr, int prosirenja) {
	string opcod = "10111"; //5 unazad
	string size = "0";
	string kodInstrukcije;
	string op1descr;

	for (vector<Instrukcija*>::iterator it = instrukcije.begin(); it != instrukcije.end(); ++it) {
		if ((*it)->ime == "call") opcode = (*it)->opcode;
		line = line.substr(4);
		if (line[0] == 'b') line = line.substr(1);
		else if (line[0] == 'w') {
			size = "1";
			line = line.substr(1);
		}
		break;
	}

	kodInstrukcije = opcod + size + "00";
	/*
	string little = kodInstrukcije.substr(8, kodInstrukcije.length() - 8);
	string big = kodInstrukcije.substr(0, 8);

	little = binToHex(little); big = binToHex(big);

	sekcija->addCode(little);
	sekcija->addCode(big); */
	sekcija->addCode(binToHex(kodInstrukcije));
	string lohi = "0";
	if (adresiranjeDst == "001") {
		if (dst[dst.length() - 1] == 'h' && !isRegister(dst)) {
			dst = dst.substr(0, dst.length() - 1);
			lohi = "1";
			if (dst[dst.length() - 1] == 'l') dst = dst.substr(0, dst.length() - 1);

		}
	}

	if (isRegister(dst)) regDst = dst;

	string regDstCode = "0000";
	if (adresiranjeDst == "001" || adresiranjeDst == "010" || adresiranjeDst == "011" || adresiranjeDst == "100")
		regDstCode = registerCode(regDst);


	if (adresiranjeDst == "") adresiranjeDst = "000";

	op1descr = adresiranjeDst + regDstCode + lohi;
	sekcija->addCode(binToHex(op1descr));

	if (!isRegister(dst) && !isRegisterLowHigh(dst)) extendInstruction(dst, prosirenja, pcrel1, prosirenja);


}
void DrugiProlaz::iret(string line, long adr, int prosirenja) {
	string opcod = "11000"; //5 unazad
	string size = "0";
	string kodInstrukcije;

	kodInstrukcije = opcod + size + "00";

	sekcija->addCode(binToHex(kodInstrukcije));

}
void DrugiProlaz::ret(string line, long adr, int prosirenja) {
	string opcod = "11001"; //5 unazad
	string size = "0";
	string kodInstrukcije;

	kodInstrukcije = opcod + size + "00";

	sekcija->addCode(binToHex(kodInstrukcije));

}

/* trebalo bi da ne treba, uvek se upisuju nule pa moze i rucno 
void DrugiProlaz::dotAlign(int line) {}
void DrugiProlaz::dotSkip(int line) {

}
*/

DrugiProlaz::DrugiProlaz()
{
	string und = "UND";
	sekcija = new Sekcija();
	sekcija->setNaziv(und);
	adresa = 0;
	pcrel1 = false; pcrel2 = false; pcrel = false;
	greska = false;
	msg = "";
}


DrugiProlaz::~DrugiProlaz()
{
}

string DrugiProlaz::trimSpace(string str) { // trimes the white space ----->checked, possibly check again
	size_t first = str.find_first_not_of(" \n\r\t");
	if (first == string::npos)
		return "";
	size_t last = str.find_last_not_of(" \n\r\t"); //tab dodat ***check
	if (last == string::npos) return "";
	str = str.substr(0, last + 1);
	str = str.substr(first);
	return str;
}

string DrugiProlaz::trimComm(string line) { //trims the comment  -----> checked
	size_t size = line.find_first_of(';');
	if (line.npos != size) {
		line = line.substr(0, size);
	}
	return line;
}

vector<string> DrugiProlaz::tokenize(string s, char c) {
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

string DrugiProlaz::binToHex(string bin) {
	string hexa = "";
	if (bin.find("0000") == 0) hexa = "0";
	else if (bin.find("0001") == 0) hexa = "1";
	else if(bin.find("0010") == 0) hexa = "2";
	else if(bin.find("0011") == 0) hexa = "3";
	else if(bin.find("0100") == 0) hexa = "4";
	else if(bin.find("0101") == 0) hexa = "5";
	else if(bin.find("0110") == 0) hexa = "6";
	else if(bin.find("0111") == 0) hexa = "7";
	else if(bin.find("1000") == 0) hexa = "8";
	else if(bin.find("1001") == 0) hexa = "9";
	else if(bin.find("1010") == 0) hexa = "A";
	else if(bin.find("1011") == 0) hexa = "B";
	else if(bin.find("1100") == 0) hexa = "C";
	else if(bin.find("1101") == 0) hexa = "D";
	else if(bin.find("1110") == 0) hexa = "E";
	else if(bin.find("1111") == 0) hexa = "F";
		else {
		cout << "Pogresan binarni format za konverziju " << bin << endl;
		getchar();
		exit(1);
		}
	bin = bin.substr(4, 4); 
	if (bin.find("0000") == 0) hexa = hexa + "0";
	else if (bin.find("0001") == 0) hexa = hexa + "1";
	else if (bin.find("0010") == 0) hexa = hexa + "2";
	else if (bin.find("0011") == 0) hexa = hexa + "3";
	else if (bin.find("0100") == 0) hexa = hexa + "4";
	else if (bin.find("0101") == 0) hexa = hexa + "5";
	else if (bin.find("0110") == 0) hexa = hexa + "6";
	else if (bin.find("0111") == 0) hexa = hexa + "7";
	else if (bin.find("1000") == 0) hexa = hexa + "8";
	else if (bin.find("1001") == 0) hexa = hexa + "9";
	else if (bin.find("1010") == 0) hexa = hexa + "A";
	else if (bin.find("1011") == 0) hexa = hexa + "B";
	else if (bin.find("1100") == 0) hexa = hexa + "C";
	else if (bin.find("1101") == 0) hexa = hexa + "D";
	else if (bin.find("1110") == 0) hexa = hexa + "E";
	else if (bin.find("1111") == 0) hexa = hexa + "F";
	else {
		cout << "Pogresan binarni format za konverziju " << bin << endl;
		getchar();
		exit(1);
	}

	return hexa;
}

