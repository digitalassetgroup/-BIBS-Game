#pragma once
#ifndef NodeEffectGrouping_h
#define NodeEffectGrouping_h

#include "Core/CommonMacros.h"
#include "Dragon/effects/effect_manager.h"
#include <vector>


class NodeEffectGrouping : public INodeEffect, public IGameEffectNotifier
{
	private_ std::vector<INodeEffect*> _initialEffects;
	private_ std::vector<INodeEffect*> _currentEffectsStack;
	private_ bool _isAboutToRemove;
	private_ bool _isComplete;

	public_ NodeEffectGrouping(cocos2d::CCNode& nodeToEffect);
	public_ ~NodeEffectGrouping();
	virtual NodeEffectGrouping* clone() const;

	public_ void AddEffect(INodeEffect* effect);
	virtual void onStart();
	virtual bool isComplete() const;

	private_ void NextEffect();

	//IGameEffectNotifier
	void OnEffectStart(IGameEffect* effect);
	void OnEffectFinish(IGameEffect* effect);

	//
	virtual void OnBeforeDestroy();
	virtual void onUpdate(const float deltaTime);
};

#endif
