#include <vector>

class shop {
private:
	std::vector<item> itemlist;
public:
	shop();
	//상점 호출 시 
	void Enter(Player& player);
	// 상점 벡터의 아이템 -> 인벤토리 벡터의 아이템으로
	void BuyItem(Player& player);
	// 인벤토리 벡터의 아이템 -> 삭제 or 상점 벡터의 아이템으로
	void SellItem(Player& player);
};