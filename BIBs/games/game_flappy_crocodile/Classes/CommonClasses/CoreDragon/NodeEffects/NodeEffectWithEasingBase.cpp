#include "NodeEffectWithEasingBase.h"
#include "Core/MathHelper.h"

NodeEffectWithEasingBase::NodeEffectWithEasingBase(
	cocos2d::CCNode& nodeToEffect, float timeInSeconds)
	: INodeEffect(nodeToEffect), _totalTime(timeInSeconds),
	_percentage(0), _easingType(DracoGames::EasingCurveLinear)
{
}

NodeEffectWithEasingBase* NodeEffectWithEasingBase::SetEasingType(
	DracoGames::EasingCurveType easingType)
{
	_easingType = easingType;
	return this; 
}

void NodeEffectWithEasingBase::onStart()
{
	_percentage = 0;
	INodeEffect::onStart();
}

bool NodeEffectWithEasingBase::isComplete() const
{
	return (_percentage >= 1.0f) ? true : false;
}

void NodeEffectWithEasingBase::onUpdate(const float deltaTime)
{
	_percentage = Math::Clamp01(_percentage + deltaTime/_totalTime);
}