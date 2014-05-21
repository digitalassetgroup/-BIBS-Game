#include "node_effect_button_scale.h"

#include "../utilities/ig_defines.h"
#include "../utilities/ig_assert.h"
#include "../utilities/ig_utilities.h"
#include "Core/MathHelper.h"

#include "cocos2d.h"
using namespace cocos2d;
using namespace DracoGames;
using namespace DracoGames::Utils;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeEffectButtonScale::NodeEffectButtonScale(cocos2d::CCNode& nodeToEffect, const float desiredScale, 
											 const float timeInSeconds) :
	INodeEffect(nodeToEffect),
	m_totalTime(timeInSeconds), m_timer(0),
	_initialScale(nodeToEffect.getScaleX(), nodeToEffect.getScaleY()),
	_finalScale(desiredScale, desiredScale),
	_easingType(EasingCurveNone), _backOvershoot(0),
	_percentage(0), _isToScaleAll(true)
{
	assert_if(m_timer < 0.0f, "Unexpected; m_timer(%f) should be >= 0.0f at this time.\n", m_timer);
	assert_if(m_totalTime < 0.0f, "Unexpected; m_totalTime(%f) should be >= 0.0f at this time.\n", m_totalTime);
}

NodeEffectButtonScale::NodeEffectButtonScale(cocos2d::CCNode& nodeToEffect, 
											 const float desiredScaleX, const float desiredScaleY,
											 const float timeInSeconds) :
	INodeEffect(nodeToEffect),
	m_totalTime(timeInSeconds),
	_initialScale(nodeToEffect.getScaleX(), nodeToEffect.getScaleY()),
	_finalScale(desiredScaleX, desiredScaleY),
	m_timer(0),
	_easingType(EasingCurveNone), _backOvershoot(0),
	_percentage(0), _isToScaleAll(false)
{
	assert_if(m_timer < 0.0f, "Unexpected; m_timer(%f) should be >= 0.0f at this time.\n", m_timer);
	assert_if(m_totalTime < 0.0f, "Unexpected; m_totalTime(%f) should be >= 0.0f at this time.\n", m_totalTime);
}

NodeEffectButtonScale* NodeEffectButtonScale::clone() const
{
	auto effect = new NodeEffectButtonScale(m_node, _finalScale.x, _finalScale.y, m_totalTime);
	effect->SetEasingType(_easingType, _backOvershoot);
	effect->SetLoopCount(GetLoopCount());
	return effect;
}

NodeEffectButtonScale* NodeEffectButtonScale::SetEasingType(DracoGames::EasingCurveType easingType, float backOvershoot) 
{ 
	_easingType = easingType; 
	_backOvershoot = backOvershoot; 
	return this; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonScale::onStart(void)
{
	m_timer = m_totalTime;
	_percentage = 0;

	assert_if(m_timer < 0.0f, "Unexpected; m_timer(%f) should be >= 0.0f at this time.\n", m_timer);
	assert_if(m_timer > m_totalTime, "Unexpected; m_timer(%f) should be <= m_totalTime(%f).\n", m_timer, m_totalTime);	
	INodeEffect::onStart();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonScale::onFinish(void)
{
	if (_isToScaleAll)
	{
		m_node.setScale(_finalScale.x);
	}
	else
	{
		m_node.setScaleX(_finalScale.x);
		m_node.setScaleY(_finalScale.y);
	}
	INodeEffect::onFinish();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonScale::CalculateTotalTime(const float deltaTime)
{
	assert_if(m_timer < 0.0f, "Unexpected; m_timer(%f) should be >= 0.0f at this time.\n", m_timer);
	assert_if(m_timer > m_totalTime, "Unexpected; m_timer(%f) should be <= m_totalTime(%f).\n", m_timer, m_totalTime);	
	
	m_timer -= deltaTime;

	if (_easingType != EasingCurveNone)
		_percentage = Math::Clamp01(_percentage + deltaTime/m_totalTime);
}


void NodeEffectButtonScale::onUpdate(const float deltaTime)
{	
	CalculateTotalTime(deltaTime);

	if (_isToScaleAll)
	{
		m_node.setScale(GetNewScale(_initialScale.x, _finalScale.x));
	}
	else
	{
		m_node.setScaleX(GetNewScale(_initialScale.x, _finalScale.x));
		m_node.setScaleY(GetNewScale(_initialScale.y, _finalScale.y));
	}
}

float NodeEffectButtonScale::GetNewScale(float startScale, float endScale)
{
	if (_easingType == EasingCurveNone)
	{
		const float percentage(Interp_CatMullRom(1.0f - Clamp(m_timer / m_totalTime, 0.0f, 1.0f), -20.0f, 0.0f, 1.0f, 1.0f));
		float newScale = ((endScale - startScale) * percentage) + startScale;
		return newScale;
	}
	else
	{
		if (_backOvershoot != 0.0f) EasingCurves::push_overshoot(_backOvershoot);
		float newScale = EasingCurves::easing_interpolate(startScale, endScale, _percentage, _easingType);
		if (_backOvershoot != 0.0f) EasingCurves::pop_overshoot();
		return newScale;
	}
}

float NodeEffectButtonScale::GetNewScaleHalf(float startScale, float endScale)
{
	float perc = (_percentage < 0.5f) ? (_percentage * 2) : (2 - _percentage * 2);

	if (_backOvershoot != 0.0f) EasingCurves::push_overshoot(_backOvershoot);
	float newScale = EasingCurves::easing_interpolate(startScale, endScale, perc, _easingType);
	if (_backOvershoot != 0.0f) EasingCurves::pop_overshoot();
	return newScale;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool NodeEffectButtonScale::isComplete(void) const
{
	return (m_timer < 0.0f) ? true : false;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
