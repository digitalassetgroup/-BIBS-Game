#pragma once
#ifndef NodeEffectRotating_h
#define NodeEffectRotating_h

#include "Core/CommonMacros.h"
#include "NodeEffectWithEasingBase.h"


//TODO: make universal rotating class like NodeEffectButtonScalePercent
class NodeEffectQuarterRotating : public NodeEffectWithEasingBase
{
	private_ const float _initialAngle;
	private_ const float _deltaAngle;

	public_ NodeEffectQuarterRotating(cocos2d::CCNode& nodeToEffect, float timeInSeconds, 
		float initialAngle, float deltaAngle);

	virtual NodeEffectQuarterRotating* clone() const;
	virtual void onFinish(void);
	virtual void onUpdate(const float deltaTime);

	private_ float GetQuarterPercentReal();
	private_ float GetNewRotationQuarter(float initial, float phase1, float phase2);
};

#endif
