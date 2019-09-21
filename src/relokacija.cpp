#include "relokacija.h"


Relokacija::Relokacija(){
}
void Relokacija::setRbr(int rbr) {
	this->rbr = rbr;
}
int Relokacija::getRbr() {
	return rbr;
}
void Relokacija::setAdr(std::string adr) {
	this->adresa = adr;
}
std::string Relokacija::getAdr() {
	return adresa;
}
void Relokacija::setTip(std::string t) {
	tip = t;
}
std::string Relokacija::getTip() {
	return tip;
}


