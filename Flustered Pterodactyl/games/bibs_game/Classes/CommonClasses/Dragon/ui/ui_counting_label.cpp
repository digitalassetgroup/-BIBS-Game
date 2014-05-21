#include "ui_counting_label.h"
#include "../utilities/ig_utilities.h"

using namespace cocos2d;

namespace ig_interface
{

	const float CountingLabel::DefaultCountTime(0.75f);
	const float CountingLabel::InstantCountTime(-1.0f);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CountingLabel::CountingLabel(const int value, const FontSizeEnum& fontSize) :
		Label(DracoGames::Utils::ToString(value), fontSize, -1.0f, ccp(0.0f, 0.0f)),	
		m_desiredValue(value),
		m_currentValue(value),
		m_totalTimeToCount(0.75f),
		m_countingTimer(-1.0f)
	{
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CountingLabel* CountingLabel::makeCountingLabel(const int value, const FontSizeEnum& fontSize)
	{		
		return new CountingLabel(value, fontSize);
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CountingLabel::~CountingLabel(void)
	{
		
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void CountingLabel::update(float deltaTime)
	{
		if(m_countingTimer >= 0.0f)
		{
			m_countingTimer -= deltaTime;
			
			if(m_countingTimer < 0.0f)
			{
				m_currentValue = m_desiredValue;
			}

			setString(DracoGames::Utils::ToString(computeDisplayedValue()));
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void CountingLabel::setDesiredValue(const int desiredValue, const float timeToDesired, const bool additive)
	{
		m_currentValue = m_desiredValue;
		m_desiredValue = desiredValue;
		
		/* On second thought, maybe additive counting is not possible. */
		assert_if(true == additive, "Invalid value for parameter; additive(%d) - Not yet implemented.", additive);

		/* The following would only be valid if additive is false. */
		m_totalTimeToCount = timeToDesired;
		m_countingTimer = m_totalTimeToCount;
		
		if(timeToDesired < 0.0f)
		{	/* Instantly jump to the desiredValue. */
			m_currentValue = m_desiredValue;
			setString(DracoGames::Utils::ToString(computeDisplayedValue()));
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	int CountingLabel::computeDisplayedValue(void) const
	{
		if(m_countingTimer < 0.0f)
		{
			assert_if(m_currentValue != m_desiredValue, "It is expected that currentValue(%d) == desiredValue(%d).\n", m_currentValue, m_desiredValue);
			return m_currentValue;
		}

		const float percentage(1.0f - DracoGames::Utils::Clamp(m_countingTimer / m_totalTimeToCount, 0.0f, 1.0f));
		return m_currentValue + static_cast<int>(percentage * (m_desiredValue - m_currentValue));
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
}; /* namespace ig_interface */
