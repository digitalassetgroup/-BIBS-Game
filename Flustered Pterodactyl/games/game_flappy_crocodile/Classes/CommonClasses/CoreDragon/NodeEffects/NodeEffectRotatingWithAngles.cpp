#include "NodeEffectRotatingWithAngles.h"
#include "base_nodes/CCNode.h"
#include <algorithm>

NodeEffectRotatingWithAngles::NodeEffectRotatingWithAngles(cocos2d::CCNode& nodeToEffect, 
	float timeBetweenChangingAngles)
	: INodeEffect(nodeToEffect), _timeBetweenChangingAngles(timeBetweenChangingAngles),
	_initialAngle(nodeToEffect.getRotation()),
	_currentTime(0)
{
}

NodeEffectRotatingWithAngles* NodeEffectRotatingWithAngles::clone() const
{
	auto effect = new NodeEffectRotatingWithAngles(m_node, _timeBetweenChangingAngles);
	effect->SetAngles(_initialDeltaAngles);
	effect->SetLoopCount(GetLoopCount());
	return effect;
}

void NodeEffectRotatingWithAngles::SetAngles(const float* anglesBegin, const float* anglesEnd)
{
	_initialDeltaAngles.assign(anglesBegin, anglesEnd);
}

void NodeEffectRotatingWithAngles::SetAngles(const std::vector<float>& angles)
{
	_initialDeltaAngles = angles;
}

void NodeEffectRotatingWithAngles::AddAngle(float angle)
{
	_initialDeltaAngles.push_back(angle);
}

void NodeEffectRotatingWithAngles::onStart(void)
{
	_leftDeltaAngles.clear();
	_leftDeltaAngles.assign(_initialDeltaAngles.begin(), _initialDeltaAngles.end());
	std::reverse(_leftDeltaAngles.begin(), _leftDeltaAngles.end());

	_currentTime = _timeBetweenChangingAngles;

	//first angle
	SetNextAngle();

	INodeEffect::onStart();
}

bool NodeEffectRotatingWithAngles::isComplete(void) const
{
	return _leftDeltaAngles.empty();
}

void NodeEffectRotatingWithAngles::onFinish(void)
{
	m_node.setRotation(_initialAngle + _initialDeltaAngles.back());
	INodeEffect::onFinish();
}

void NodeEffectRotatingWithAngles::onUpdate(const float deltaTime)
{
	_currentTime -= deltaTime; //yield wanted :'(
	if (_currentTime < 0 && !_leftDeltaAngles.empty())
	{
		_currentTime = _timeBetweenChangingAngles + _currentTime;
		SetNextAngle();
	}
}

void NodeEffectRotatingWithAngles::SetNextAngle()
{
	auto nextAngle = _leftDeltaAngles.back();
	_leftDeltaAngles.pop_back();
	m_node.setRotation(_initialAngle + nextAngle);
}