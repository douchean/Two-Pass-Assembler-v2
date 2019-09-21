#pragma once
#include <string>
class Relokacija
{
private:
	int rbr;
	std::string adresa;
	std::string tip;

public:
	void setRbr(int rbr);
	int getRbr();
	void setAdr(std::string adr);
	std::string getAdr();
	void setTip(std::string t);
	std::string getTip();
	Relokacija();
};

