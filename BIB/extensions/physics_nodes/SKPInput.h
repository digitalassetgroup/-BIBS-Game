#pragma once

#ifndef __SKP_HERO_INPUT_H__
#define __SKP_HERO_INPUT_H__

enum SKPInputControl
{
	SKPICLeft,
	SKPICRight,
	SKPICJump,
	SKPICDuck,
	SKPICAction,
	SKPICTotal
};

class SKPInput
{
public:
	
	SKPInput()
	{
		for (int i = 0; i < SKPICTotal; ++i)
		{
			states[i] = false;
		}
	}
	
	bool hasDone(SKPInputControl control)
	{
		return states[control] && !getInputState(control);
	}
	
	bool isDoing(SKPInputControl control)
	{
		return states[control] && getInputState(control);
	}
		
	bool justDid(SKPInputControl control)
	{
		return !states[control] && getInputState(control);
	}

	void updateInput()
	{
		for (int i = 0; i < SKPICTotal; ++i)
		{
			states[i] = getInputState((SKPInputControl)i);
		}
	};
	
	virtual bool getInputState(SKPInputControl control) = 0;
private:
	bool states[SKPICTotal];
};

#endif //__SKP_HERO_INPUT_H__