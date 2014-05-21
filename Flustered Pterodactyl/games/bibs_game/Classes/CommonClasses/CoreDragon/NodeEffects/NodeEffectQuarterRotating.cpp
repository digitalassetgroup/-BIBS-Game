#include "NodeEffectQuarterRotating.h"
using namespace DracoGames;


NodeEffectQuarterRotating::NodeEffectQuarterRotating(cocos2d::CCNode& nodeToEffect, 
	float timeInSeconds, float initialAngle, float deltaAngle)
	: NodeEffectWithEasingBase(nodeToEffect, timeInSeconds), _initialAngle(initialAngle), _deltaAngle(deltaAngle)
{
}

NodeEffectQuarterRotating* NodeEffectQuarterRotating::clone() const
{
	auto effect = new NodeEffectQuarterRotating(m_node, _totalTime, _initialAngle, _deltaAngle);
	effect->SetLoopCount(GetLoopCount());
	effect->SetEasingType(_easingType);
	return effect;
}

void NodeEffectQuarterRotating::onFinish()
{
	m_node.setRotation(_initialAngle);
	NodeEffectWithEasingBase::onFinish();
}

void NodeEffectQuarterRotating::onUpdate(const float deltaTime)
{
	NodeEffectWithEasingBase::onUpdate(deltaTime);
	m_node.setRotation(GetNewRotationQuarter(_initialAngle, _initialAngle - _deltaAngle, _initialAngle + _deltaAngle));
}

//TODO: вынести следующие 2 функции в Math
float NodeEffectQuarterRotating::GetQuarterPercentReal()
{
	float perc = 0;
	if (_percentage < 0.25f) perc = _percentage * 4;
	else if (_percentage < 0.5f) perc = 2 - _percentage * 4;
	else if (_percentage < 0.75f) perc = (_percentage - 0.5f) * 4;
	else perc = 2 - (_percentage - 0.5f) * 4;

	float percentReal = EasingCurves::eval(perc, _easingType);

	return percentReal;
}

float NodeEffectQuarterRotating::GetNewRotationQuarter(float initial, float phase1, float phase2)
{
	float percentReal = GetQuarterPercentReal();

	float newScale = 0;
	if (_percentage < 0.5f)
		newScale = initial + percentReal * (phase1 - initial);
	else
		newScale = initial + percentReal * (phase2 - initial);

	return newScale;
}