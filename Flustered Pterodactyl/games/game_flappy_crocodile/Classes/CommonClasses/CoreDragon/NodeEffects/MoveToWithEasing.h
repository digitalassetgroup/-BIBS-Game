#pragma once
#ifndef MoveToWithEasing_h
#define MoveToWithEasing_h

#include "Core/CommonMacros.h"
#include "Dragon/utilities/ig_easing_curves.h"
#include "actions/CCActionInterval.h"
using namespace cocos2d;


class MoveToWithEasing : public CCActionInterval
{
private:
	MoveToWithEasing();

public:	
    static MoveToWithEasing* create(float duration, const CCPoint& position);
    bool initWithDuration(float duration, const CCPoint& position);

    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode* target);
    virtual void update(float time);

	MoveToWithEasing* SetEasingType(DracoGames::EasingCurveType easingType, float backOvershoot = 0.0f);
	MoveToWithEasing* WithNode(CCNode* node);

private:
	CCNode* GetTarget();
    
protected:
	float m_elapsedManual; //using in CCSequnece by children
    CCPoint m_endPosition;
    CCPoint m_startPosition;
    CCPoint m_delta;
	DracoGames::EasingCurveType _easingType;
	float _easingBackOvershoot;
	CCNode* _node;
};

#endif
