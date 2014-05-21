#pragma once
#ifndef NodeEffectWithEasingBase_h
#define NodeEffectWithEasingBase_h

#include "Core/CommonMacros.h"
#include "Dragon/effects/effect_manager.h"
#include "Dragon/utilities/ig_easing_curves.h"


class NodeEffectWithEasingBase : public INodeEffect
{
	protected_ const float _totalTime;
	protected_  DracoGames::EasingCurveType _easingType;
	protected_ float _percentage;

	public_ NodeEffectWithEasingBase(cocos2d::CCNode& nodeToEffect, float timeInSeconds);
	public_ NodeEffectWithEasingBase* SetEasingType(DracoGames::EasingCurveType easingType);
	virtual void onStart();
	virtual bool isComplete() const;
	virtual void onUpdate(const float deltaTime);
};

#endif
