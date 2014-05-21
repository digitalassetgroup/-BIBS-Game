#pragma once
#ifndef __cannonboy__node_effect_sine_wave_motion
#define __cannonboy__node_effect_sine_wave_motion "$Rev: 2004 $"

#include "effect_manager.h"
#include "../utilities/ig_one_time_const.h"
#include "../utilities/ig_easing_curves.h"
#include "cocos2d.h"

class NodeEffectSineWaveMotion : public INodeEffect
{
public:
	/**
	 * @param maxDeviation - max distance between initial and current position
	 */
	NodeEffectSineWaveMotion(cocos2d::CCNode& nodeToEffect,
							 const float maxDeviation,
							 const float period);
	
	/**
	 * @param wavePhase - wave animation phase in radians
	 */
	void setPhase(const float wavePhase);
	
private:
	virtual void onStart(void);
	virtual void onFinish(void);
	virtual void onUpdate(const float deltaTime);
	virtual bool isComplete(void) const;
	
	const float m_period;
	const float m_maxDeviation;
	const cocos2d::CCPoint m_startPosition;
	
	float m_wavePhase;
};

#endif /* defined(__cannonboy__node_effect_sine_wave_motion) */
