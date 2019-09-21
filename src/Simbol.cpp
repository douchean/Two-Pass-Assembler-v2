#include "simbol.h"



Simbol::Simbol()
{
	global = false; def = false; extrn = false; con = false; sekcija = NULL;
}


Simbol::~Simbol()
{
}

bool Simbol::isDefined() {
	return this->def;
}

bool Simbol::isGlobal() {
	return this->global;
}

bool Simbol::isExtern() {
	return this->extrn;
}

bool Simbol::isConst() {
	return this->con;
}
