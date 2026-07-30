#pragma once
#include <iostream>
#include <string>

class Player;

class Monster {

protected:
	std::string name;
	std::string property;
	std::int hp;
	std::int atk;

public:

	Monster(string name, string property, int hp, int atk);

	virtual Monster() {};

	string getname() { return name; }
	void setname(string name) { this->name = name; }

	string getproperty() { return property; }
	void setproperty()(string name) {this->property = property; }

	int gethp() { return hp; }
	void sethp(int hp) { int hp } {this->hp = hp; }

	int getatk() { return atk; }
	void setatk(int atk) { int atk } {this->atk = atk; }


};