#pragma once
#ifndef _DracoGames_NodeEffectButtonScale_h_
#define _DracoGames_NodeEffectButtonScale_h_

#include "effect_manager.h"
#include "../utilities/ig_one_time_const.h"
#include "../utilities/ig_easing_curves.h"
#include "cocos2d.h"

class NodeEffectButtonScale : public INodeEffect
{
public:
	NodeEffectButtonScale(cocos2d::CCNode& nodeToEffect, const float desiredScale, const float timeInSeconds);
	NodeEffectButtonScale(cocos2d::CCNode& nodeToEffect,  const float desiredScaleX, const float desiredScaleY, const float timeInSeconds);

	NodeEffectButtonScale* SetEasingType(DracoGames::EasingCurveType easingType, float backOvershoot = 0.0f);

	virtual NodeEffectButtonScale* clone() const;

protected:
	virtual void onStart(void);
	virtual void onFinish(void);
	
	void CalculateTotalTime(const float deltaTime);

	virtual void onUpdate(const float deltaTime);
	virtual bool isComplete(void) const;
	
	float GetNewScale(float startScale, float endScale);
	float GetNewScaleHalf(float startScale, float endScale);	

	float m_timer;
	const CCPoint _initialScale;
	const CCPoint _finalScale;
	const float m_totalTime;

	DracoGames::EasingCurveType _easingType;
	float _backOvershoot;
	float _percentage;
	const bool _isToScaleAll; //scale x and y
};


#endif /* _DracoGames_NodeEffectButtonScale_h_ */
