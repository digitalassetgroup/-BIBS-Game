#pragma once
#ifndef NodeEffectRotatingWithAngles_h
#define NodeEffectRotatingWithAngles_h

#include "Core/CommonMacros.h"
#include "Dragon/effects/effect_manager.h"
#include <vector>


class NodeEffectRotatingWithAngles : public INodeEffect
{
	private_ std::vector<float> _initialDeltaAngles;
	private_ const float _initialAngle;
	private_ const float _timeBetweenChangingAngles;

	private_ std::vector<float> _leftDeltaAngles;	
	private_ float _currentTime;

	public_ NodeEffectRotatingWithAngles(cocos2d::CCNode& nodeToEffect, float timeBetweenChangingAngles);
	virtual NodeEffectRotatingWithAngles* clone() const;

	public_ void SetAngles(const float* anglesBegin, const float* anglesEnd);
	public_ void SetAngles(const std::vector<float>& angles);
	public_ void AddAngle(float angle);

	virtual void onStart(void);
	virtual bool isComplete(void) const;
	virtual void onFinish(void);
	virtual void onUpdate(const float deltaTime);

	private_ void SetNextAngle();
};

#endif
