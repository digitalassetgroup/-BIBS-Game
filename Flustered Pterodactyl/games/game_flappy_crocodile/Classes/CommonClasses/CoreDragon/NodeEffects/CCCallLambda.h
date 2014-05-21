#pragma once
#ifndef CCCallLambda_h
#define CCCallLambda_h

#include "Core/CommonMacros.h"
#include "Core/QFunction.h"
#include "actions/CCActionInterval.h"
#include "Dragon/utilities/ig_easing_curves.h"
using namespace cocos2d;
using namespace DracoGames;


class CCCallLambda : public CCActionInterval
{
	private_ QFunction<void()> _funcEnd;
	private_ bool _isfuncEndCalled;

	private_ CCCallLambda();
	public_ virtual void update(float time);
	

	public_ template<typename TFunc>
	static CCCallLambda* create(float duration, const TFunc& funcEnd) 
	{
		auto result = new CCCallLambda();
		result->initWithDuration(duration);
		result->autorelease();

		result->_funcEnd = funcEnd;
		return result;
	}	
};


class CCCallLambdaInstant : public CCActionInstant
{
private:
	QFunction<void()> _func;

private:
	CCCallLambdaInstant();

public:
	virtual void execute();
	virtual void update(float time);

	template<typename TFunc>
	static CCCallLambdaInstant* create(const TFunc& func) 
	{
		auto result = new CCCallLambdaInstant();
		result->autorelease();
		result->_func = func;
		return result;
	}		
};


class CCCallLambdaUpdate : public CCAction
{
	private_ QFunction<bool()> _funcUpdate;
	private_ QFunction<void()> _funcEnd;
	private_ bool _isDone;
	private_ bool _isFirstStepCompleted;

	private_ CCCallLambdaUpdate();
	public_ ~CCCallLambdaUpdate();
	public_ virtual bool isDone(void);
	public_ virtual void step(float dt);

	public_ template<typename TFuncUpdate, typename TFuncEnd>
	static CCCallLambdaUpdate* create(const TFuncUpdate& funcUpdate, const TFuncEnd& funcEnd) 
	{
		auto result = new CCCallLambdaUpdate();
		result->autorelease();

		result->_funcUpdate = funcUpdate;
		result->_funcEnd = funcEnd;
		return result;
	}
};


class CCCallLambdaEasingUpdate : public CCActionInterval
{
	private_ QFunction<void(float)> _funcSetter;
	private_ float _valueFrom;
	private_ float _valueTo;
	private_ EasingCurveType _easingType;
    private_ float _easingBackOvershoot;
	public_ CCCallLambdaEasingUpdate* SetEasingType(EasingCurveType easingType, float backOvershoot);

	private_ CCCallLambdaEasingUpdate();

	public_ template<typename TSetter>
	static CCCallLambdaEasingUpdate* create(float duration, float valueFrom, float valueTo, const TSetter& funcSetter) 
	{
		auto result = new CCCallLambdaEasingUpdate();
		result->initWithDuration(duration);
		result->autorelease();

		result->_valueFrom = valueFrom;
		result->_valueTo = valueTo;
		result->_funcSetter = funcSetter;
		return result;
	}

	private_ virtual void update(float percent);
};


#endif
