#include "battle.h"
#include "player.h"
#include "monster.h"
#include <iostream>





void StartBattle(Player* player, Monster* monster) {
	while (player->getHp() > 0 && monster->getHp() > 0) {

		
		std::cout << "1. 공격	2. 아이템 사용" << std::endl;
		std::cout << "선택: ";

		int choice;
		std::cin >> choice;

		if (choice == 1) {
			player->Attack(monster);
		}
		else if (choice == 2) {
			std::cout << "아이템 사용" (어떻게 구현할지 고민중) << std::endl;
		}
		// else if (choice == 3) {
			//std::cout << "도망쳤다!" << std::endl;
			//return;}
		else {
			std::cout << "잘못된 입력이에요. 다시 선택하세요." << std::endl;
			continue;
		}

		if (monster->getHp() <= 0) {
			break;
		}





	if (player->getHp() <= 0) {

		std::cout << "분리수거 실패" << std::endl;

	}
	else {
		
		std::cout << monster->getName() << "분리수거 성공!!" << std::endl;
	}

}