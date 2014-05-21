#include "CCOpacityTo.h"

CCOpacityTo::CCOpacityTo()
	: _opacityTo(), _opacityFrom(), _nodeForOpacity()
{
}

void CCOpacityTo::startWithTarget(CCNode* target)
{
	if (_nodeForOpacity == nullptr)
	{
		_nodeForOpacity = dynamic_cast<CCRGBAProtocol*>(target);
	}

	_opacityFrom = _nodeForOpacity->getOpacity();

	CCActionInterval::startWithTarget(target);
}

void CCOpacityTo::update(float time)
{
	_nodeForOpacity->setOpacity(GLubyte(_opacityFrom + (_opacityTo - _opacityFrom) * time));
}

//virtual CCActionInterval* reverse(void);
//virtual CCObject* copyWithZone(CCZone* pZone);

CCOpacityTo* CCOpacityTo::create(float d, GLubyte opacityTo, CCRGBAProtocol* nodeForOpacity)
{
	auto action = new CCOpacityTo();
	action->_opacityTo = opacityTo;
	action->_nodeForOpacity = nodeForOpacity;

	action->initWithDuration(d);
	action->autorelease();

	return action;
}