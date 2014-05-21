#include "CCMessageBoxYesNo.h"
#include "Core/MathHelper.h"

std::map<int, QFunction<void(bool)> > CCMessageBoxYesNo::_callbacks;

CCMessageBoxYesNo* CCMessageBoxYesNo::Inst()
{
	static CCMessageBoxYesNo box;
	return &box;
}

void CCMessageBoxYesNo::Show(const std::string& message, const std::string& title, const QFunction<void()>& callbackOnYes)
{
	Show(message, title, "Yes", "No", callbackOnYes);
}

void CCMessageBoxYesNo::ShowWithBool(const std::string& message, const std::string& title, const QFunction<void(bool)>& callback)
{
	ShowWithBool(message, title, "Yes", "No", callback);
}

void CCMessageBoxYesNo::Show(const std::string& message, const std::string& title, const std::string& yesText, const std::string& noText, 
	const QFunction<void()>& callbackOnYes)
{
	ShowWithBool(message, title, yesText, noText, [=](bool isYes)
	{
		if (isYes)
			callbackOnYes();
	});
}

void CCMessageBoxYesNo::ShowWithBool(const std::string& message, const std::string& title, const std::string& yesText, 
	const std::string& noText, const QFunction<void(bool)>& callback)
{
	int dialogId = Math::RandomRange(1, 100000);
	_callbacks[dialogId] = callback;
	sk::game_services::open_yes_no_dialog(dialogId, Inst(), message, title);
}

void CCMessageBoxYesNo::ui_on_yes_no_dialog(unsigned int dialog_id, bool result)
{
	auto it = _callbacks.find(dialog_id);
	if (it != _callbacks.end())
	{
		auto& callback = it->second;
		callback(result);
		_callbacks.erase(it);
	}
}