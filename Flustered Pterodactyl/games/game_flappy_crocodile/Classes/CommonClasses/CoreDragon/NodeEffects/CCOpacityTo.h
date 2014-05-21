#pragma once
#ifndef CCOpacityTo_h
#define CCOpacityTo_h

#include "Core/CommonMacros.h"
#include "actions/CCActionInterval.h"
using namespace cocos2d;


class CCOpacityTo : public CCActionInterval
{
	private_ GLubyte _opacityTo;
	private_ GLubyte _opacityFrom;
	private_ CCRGBAProtocol* _nodeForOpacity;

	private_ CCOpacityTo();
	public_ virtual void startWithTarget(CCNode* target);
	public_ virtual void update(float time);

	//virtual CCActionInterval* reverse(void);
	//virtual CCObject* copyWithZone(CCZone* pZone);
	public_ static CCOpacityTo* create(float d, GLubyte opacityTo, CCRGBAProtocol* nodeForOpacity = nullptr);
};

#endif
