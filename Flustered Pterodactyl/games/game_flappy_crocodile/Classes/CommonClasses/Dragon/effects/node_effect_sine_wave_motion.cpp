#include "node_effect_sine_wave_motion.h"

NodeEffectSineWaveMotion::NodeEffectSineWaveMotion(cocos2d::CCNode& nodeToEffect,
												   const float maxDeviation,
												   const float period) :
	INodeEffect(nodeToEffect),
	m_maxDeviation(maxDeviation),
	m_startPosition(nodeToEffect.getPosition()),
	m_period(period),
	m_wavePhase(0.0f)
{
	assert_if(m_period <= 0.0f, "Period should be more than 0.\n");
}

void NodeEffectSineWaveMotion::setPhase(const float wavePhase)
{
	m_wavePhase = wavePhase;
}

void NodeEffectSineWaveMotion::onStart()
{
	m_node.setPosition(m_startPosition);
}

void NodeEffectSineWaveMotion::onFinish()
{
}

void NodeEffectSineWaveMotion::onUpdate(const float deltaTime)
{
	assert_if(deltaTime < 0.0f, "Invalid value for paramater; deltaTime(%f). Expected greater than 0.0f\n", deltaTime);
	
	m_wavePhase += (deltaTime / m_period) * 2.0f * M_PI;
	const float y(m_startPosition.y + m_maxDeviation * sin(m_wavePhase));
	m_node.setPositionY(y);
}

bool NodeEffectSineWaveMotion::isComplete() const
{
	return false;
}
