#include "CCInAppPurchase.h"

std::map<int, std::queue<QFunction<void(bool)>> > CCInAppPurchase::_callbacks;

CCInAppPurchase* CCInAppPurchase::Inst()
{	
	static CCInAppPurchase inst;
	static bool isInitialized = false;
	if (!isInitialized)
	{
		isInitialized = true;
		sk::game_services::register_ui_purchase_observer(&inst);
	}
	return &inst;
}

void CCInAppPurchase::Buy(unsigned int game_id, const QFunction<void()>& callbackOnYes)
{
	BuyWithBool(game_id, [=](bool isYes)
	{
		if (isYes)
			callbackOnYes();
	});
}

void CCInAppPurchase::BuyWithBool(unsigned int game_id, const QFunction<void(bool)>& callback)
{
	Inst();

	if (_callbacks.find(game_id) == _callbacks.end())
	{
		_callbacks[game_id] = std::queue<QFunction<void(bool)>>();
	}
	
	_callbacks[game_id].push(callback);
	sk::game_services::buy_product(game_id);
}

void CCInAppPurchase::ui_on_purchase(unsigned int game_id, bool purchased)
{
	auto it = _callbacks.find(game_id);
	if (it == _callbacks.end())
		return;

	auto& callback = it->second.front();

	callback(purchased);

	it->second.pop();
	if (it->second.empty())
	{
		_callbacks.erase(it);
	}
}

//just for testing
void CCInAppPurchase::TestOnPurchase(unsigned int game_id)
{
	Inst()->ui_on_purchase(game_id, true);
}

