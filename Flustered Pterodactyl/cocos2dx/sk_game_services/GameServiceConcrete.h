
//note: this file only can be included in game_project, NOT in cocos2d

#ifdef GameServiceConcrete_h
	#error "This file can be included only once!"
#endif
#define GameServiceConcrete_h

#include "sk_game_services.h"

class GameServicesConcrete : public sk::game_services::GameServicesBase
{
	virtual bool are_in_apps_disabled() override
	{
		return SK_NAMESPACE::game_services::are_in_apps_disabled();
	}

	virtual bool disable_offer_wall() override
	{
		return SK_NAMESPACE::game_services::disable_offer_wall();
	}

	virtual bool disable_video_ad() override
	{
		return SK_NAMESPACE::game_services::disable_video_ad();
	}

	virtual bool is_game_center_disabled() override
	{
		return SK_NAMESPACE::game_services::is_game_center_disabled();
	}

	virtual bool is_game_paid() override
	{
		return SK_NAMESPACE::game_services::is_game_paid();
	}

	virtual sk::game_services::supported_orientations get_supported_orientation() override
	{
		return SK_NAMESPACE::game_services::get_supported_orientation();
	}

	virtual int levels_per_interstitial() override
	{
		return SK_NAMESPACE::game_services::levels_per_interstitial();
	}

	virtual bool disable_all_interstitials() override
	{
		return SK_NAMESPACE::game_services::disable_all_interstitials();
	}

	virtual const char* game_title() override
	{
		return SK_NAMESPACE::game_services::game_title();
	}

	virtual void game_do_resume() override
	{
		SK_NAMESPACE::game_services::game_do_resume();
	}

	virtual void game_do_pause() override
	{
		SK_NAMESPACE::game_services::game_do_pause();
	}
};

namespace sk
{
	namespace game_services
	{
		static void SetGameServicesConcrete()
		{
			sk::game_services::GameServicesBase::SetGameServices(new GameServicesConcrete);
		}
	}
}