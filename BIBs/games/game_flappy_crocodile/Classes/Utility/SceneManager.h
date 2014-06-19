#pragma once
#ifndef SceneManager_h
#define SceneManager_h

#include "CorePlatform/SceneManagerBase.h"

namespace GameSceneType
{
	enum Type 
	{ 
		No = 0, 
	};
}


class SceneManager : public SceneManagerBase
{
public:
	static SceneManager& Inst();
	void RunAtFirst();

private:
	static void OnBeforePurgeDirector();
};

#endif
