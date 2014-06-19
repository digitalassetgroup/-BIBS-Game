#pragma once
#ifndef CCMessageBoxYesNo_h
#define CCMessageBoxYesNo_h

#include "sk_game_services/sk_game_services.h"
#include <map>
#include <string>
#include "Core/QFunction.h"


class CCMessageBoxYesNo : public sk::game_services::ui_dialog_observer
{
private:
	static std::map<int, QFunction<void(bool)> > _callbacks;

private:
	static CCMessageBoxYesNo* Inst();

public:
	static void Show(const std::string& message, const std::string& title, const QFunction<void()>& callbackOnYes);
	static void ShowWithBool(const std::string& message, const std::string& title, const QFunction<void(bool)>& callback);
	//overloaded methods with yesText and notext
	static void Show(const std::string& message, const std::string& title, const std::string& yesText, const std::string& noText, const QFunction<void()>& callbackOnYes);
	static void ShowWithBool(const std::string& message, const std::string& title, const std::string& yesText, const std::string& noText, const QFunction<void(bool)>& callback);

	virtual void ui_on_yes_no_dialog(unsigned int dialog_id, bool result) override;
};

#endif