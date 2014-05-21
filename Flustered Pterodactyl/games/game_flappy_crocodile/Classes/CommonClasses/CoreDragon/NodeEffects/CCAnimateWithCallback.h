#pragma once
#ifndef CCAnimateWithCallback_h
#define CCAnimateWithCallback_h

#include "actions/CCActionInterval.h"
#include "Core/QFunction.h"
namespace cocos2d { class CCAnimation; }
using namespace cocos2d;


class CCAnimateWithCallback : public CCAnimate
{
private:
	int _prevFrame;
	QFunction<void(int)> _funcOnFrameChanged;

public:	
	static CCAnimateWithCallback* create(CCAnimation* animation);
	CCAnimateWithCallback* SetOnFrameChanged(const QFunction<void(int)>& funcOnFrameChanged);

private:
	CCAnimateWithCallback();
	virtual void update(float dt) override;
};

#endif