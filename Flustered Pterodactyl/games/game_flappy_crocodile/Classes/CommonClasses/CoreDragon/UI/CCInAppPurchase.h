#pragma once
#ifndef CCInAppPurchase_h
#define CCInAppPurchase_h

#include <map>
#include <queue>
#include "sk_game_services/sk_game_services.h"
#include "Core/QFunction.h"


class CCInAppPurchase : public sk::game_services::ui_purchase_observer
{
private:
	static std::map<int, std::queue<QFunction<void(bool)>> > _callbacks;

private:
	static CCInAppPurchase* Inst();

public:
	static void Buy(unsigned int game_id, const QFunction<void()>& callbackOnYes);
	static void BuyWithBool(unsigned int game_id, const QFunction<void(bool)>& callback);
	static void TestOnPurchase(unsigned int game_id);

private:
	virtual void ui_on_purchase(unsigned int game_id, bool purchased) override;
};


#endif