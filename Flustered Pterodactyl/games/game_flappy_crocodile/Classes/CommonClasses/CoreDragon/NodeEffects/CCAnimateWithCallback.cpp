#include "CCAnimateWithCallback.h"


CCAnimateWithCallback* CCAnimateWithCallback::create(CCAnimation* animation)
{
	auto animate = new CCAnimateWithCallback();
	animate->initWithAnimation(animation);
	animate->autorelease();
	return animate;
}

CCAnimateWithCallback* CCAnimateWithCallback::SetOnFrameChanged(const QFunction<void(int)>& funcOnFrameChanged)
{
	_funcOnFrameChanged = funcOnFrameChanged;
	return this;
}

CCAnimateWithCallback::CCAnimateWithCallback() 
	: _prevFrame()
{
}

void CCAnimateWithCallback::update(float dt)
{
	CCAnimate::update(dt);

	if (_prevFrame != m_nNextFrame)
	{
		_funcOnFrameChanged(_prevFrame);
		_prevFrame = m_nNextFrame;			
	}
}
