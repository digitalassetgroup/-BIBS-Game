#pragma once
#ifndef NodeEffectCirclePosition_h
#define NodeEffectCirclePosition_h

#include "Core/CommonMacros.h"
#include "NodeEffectWithEasingBase.h"
using namespace DracoGames;


class NodeEffectCirclePosition : public NodeEffectWithEasingBase
{
	protected_ CCPoint _nodeCenterPosition;
	protected_ CCPoint _distanceFromCenter;

	public_ NodeEffectCirclePosition(cocos2d::CCNode& nodeToEffect, float timeInSeconds, 
		const CCPoint& distanceFromCenter);

	public_ NodeEffectCirclePosition* SetNodeCenterDeltaPosition(const CCPoint& deltaPosition);

	virtual INodeEffect* clone() const;
	virtual void onFinish(void);
	virtual void onUpdate(const float deltaTime);
};


#endif
