#pragma once
#ifndef _DracoGames_NodeEffectButtonSlideIn_h_
#define _DracoGames_NodeEffectButtonSlideIn_h_

#include "effect_manager.h"
#include "../utilities/ig_one_time_const.h"
#include "cocos2d.h"

enum ButtonSlideDirection
{
	SLIDE_FROM_TOP,
	SLIDE_FROM_BOTTOM,
	SLIDE_FROM_LEFT,
	SLIDE_FROM_RIGHT,
	
	SLIDE_TO_TOP = SLIDE_FROM_TOP,
	SLIDE_TO_BOTTOM,
	SLIDE_TO_LEFT,
	SLIDE_TO_RIGHT,	
};

class NodeEffectButtonSlideIn : public INodeEffect
{
public:
	static const float SceneStartDelay;
	static const float PerNodeDelay;
	static inline float delayOfNode(const size_t index) { return SceneStartDelay + (PerNodeDelay * index); }
	
	NodeEffectButtonSlideIn(cocos2d::CCNode& nodeToEffect, const ButtonSlideDirection& fromDirection, const float slideDelay = -1.0f);
	virtual NodeEffectButtonSlideIn* clone() const;

protected:
	cocos2d::CCPoint m_startPosition;
	cocos2d::CCPoint m_finalPosition;
	
private:
	virtual void onStart(void);
	virtual void onFinish(void);	
	virtual void onUpdate(const float deltaTime);
	virtual bool isComplete(void) const;
	
	float m_moveToTimer;
	float m_delayTimer;
};

class NodeEffectButtonSlideOut : public NodeEffectButtonSlideIn
{
public:	
	NodeEffectButtonSlideOut(cocos2d::CCNode& nodeToEffect, const ButtonSlideDirection& towardDirection, const float slideDelay = -1.0f);
};

#endif /* _DracoGames_NodeEffectButtonSlideIn_h_ */
