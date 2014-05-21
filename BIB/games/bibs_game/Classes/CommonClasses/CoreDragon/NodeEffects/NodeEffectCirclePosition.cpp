#include "NodeEffectCirclePosition.h"
#include "Core/MathHelper.h"
#include "CorePlatform/CCPointMath.h"

NodeEffectCirclePosition::NodeEffectCirclePosition(cocos2d::CCNode& nodeToEffect, 
	float timeInSeconds, const CCPoint& distanceFromCenter)
	: NodeEffectWithEasingBase(nodeToEffect, timeInSeconds), _distanceFromCenter(distanceFromCenter)
{
	_nodeCenterPosition = nodeToEffect.getPosition();
}

NodeEffectCirclePosition* NodeEffectCirclePosition::SetNodeCenterDeltaPosition(const CCPoint& deltaPosition)
{
	_nodeCenterPosition += deltaPosition;
	return this;
}

INodeEffect* NodeEffectCirclePosition::clone() const
{
	return NULL;
}

void NodeEffectCirclePosition::onFinish(void)
{
	NodeEffectWithEasingBase::onFinish();
}

void NodeEffectCirclePosition::onUpdate(const float deltaTime)
{
	NodeEffectWithEasingBase::onUpdate(deltaTime);
		
	float percentReal = EasingCurves::eval(_percentage, _easingType);
	float newAngle = Math::Lerp(360, 0, percentReal);

	float newAngleInRadians = Math::DegreesToRadians(newAngle);
	auto newPos = ccp(_nodeCenterPosition.x + cos(newAngleInRadians) * _distanceFromCenter.x, 
		_nodeCenterPosition.y + sin(newAngleInRadians) * _distanceFromCenter.y);

	m_node.setPosition(newPos);
}