#include."player.h"
#include."monster.h"
#include <iostream>

Monster::Monster(string name, string property, int hp, int atk) {

	this->name = name;
	this->property = property;
	this->hp = hp;
	this->atk = atk;
}


