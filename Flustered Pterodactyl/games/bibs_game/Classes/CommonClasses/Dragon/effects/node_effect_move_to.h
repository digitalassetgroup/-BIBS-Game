#pragma once
#ifndef _DracoGames_NodeEffectMoveTo_h_
#define _DracoGames_NodeEffectMoveTo_h_

#include "effect_manager.h"
#include "../utilities/ig_one_time_const.h"
#include "../utilities/ig_easing_curves.h"
#include "cocos2d.h"

class MoveToEffect : public INodeEffect
{
public:
	MoveToEffect(cocos2d::CCNode& nodeToEffect);
	
	/**
	 *	@param goalPosition is the position where the node wants to move to.
	 *	@param moveDirection must be a normalized vector pointing in the direction of motion.
	 *	@warning this will jump the node to a give position, immediately without any transition.
	 *	@note this function can only be called once, ever.
	 */
	void moveWithDirectionAndSpeed(const cocos2d::CCPoint& goalPosition, const cocos2d::CCPoint& moveDirection,
								    const float moveSpeed, const float totalTime, 
                                    DracoGames::EasingCurveType easingX = DracoGames::EasingCurveLinear, 
                                    DracoGames::EasingCurveType easingY = DracoGames::EasingCurveUnknown); // use easingX if not specified

    void moveWithDirectionAndDistance(const cocos2d::CCPoint& goalPosition, const cocos2d::CCPoint& moveDirection,
                                   const float distance, const float totalTime, 
                                   DracoGames::EasingCurveType easingX = DracoGames::EasingCurveLinear, 
                                   DracoGames::EasingCurveType easingY = DracoGames::EasingCurveUnknown); // use easingX if not specified

	/**
	 *	By default the move will start immediately, but sometimes a delay is desired; this allows the move to start
	 *	after a specified number of seconds passes.
	 */
	void setMoveDelay(const float delayInSeconds);
    void setOvershoot(const float overshoot);
private:
	virtual void onStart(void);
	virtual void onFinish(void);
	
	virtual void onUpdate(const float deltaTime);
	virtual bool isComplete(void) const;
	
	cocos2d::CCPoint m_startPosition;
	cocos2d::CCPoint m_goalPosition;
	DracoGames::igOneTimeConst<float> m_totalTime;
	float m_moveToTimer;
	float m_delayTimer;
    float m_overshoot;
    DracoGames::EasingCurveType m_easingX;
    DracoGames::EasingCurveType m_easingY;
};

#endif /* _DracoGames_NodeEffectMoveTo_h_ */
