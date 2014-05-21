#pragma once
#ifndef DracoGames_UI_COUNTING_LABEL_H
#define DracoGames_UI_COUNTING_LABEL_H "$Rev: 2188 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ui_label.h"

namespace ig_interface
{
	
	class CountingLabel : public Label
	{
	public:
		static const float DefaultCountTime;
		static const float InstantCountTime;
		
		virtual ~CountingLabel(void);
		/**
		 *	Create a label from a direct string, performing no lookup with the text translator.
		 *
		 *	@note using makeLabelWithKey is preferred to this variation.
		 */
		static CountingLabel* makeCountingLabel(const int value, const FontSizeEnum& fontSize = defaultFontSize());
		
		
		virtual void update(float deltaTime);
		
		/**
		 *	Make the 
		 */
		void setDesiredValue(const int desiredValue, const float timeToDesired = DefaultCountTime, const bool additive = false);
		
	private:
		/* Prevent accidental use.  To Create a label use the static Factory methods. */
		CountingLabel(const int value, const FontSizeEnum& fontSize);		
		CountingLabel(const CountingLabel& obj);
		CountingLabel& operator=(const CountingLabel& obj);
		
		virtual void setString(const std::string& text) { Label::setString(text); }
		int computeDisplayedValue(void) const;
		
		float m_totalTimeToCount;
		float m_countingTimer;
		int m_currentValue;
		int m_desiredValue;
	};
	
}; /* namespace ig_interface */

#endif /* DracoGames_UI_COUNTING_LABEL_H */
