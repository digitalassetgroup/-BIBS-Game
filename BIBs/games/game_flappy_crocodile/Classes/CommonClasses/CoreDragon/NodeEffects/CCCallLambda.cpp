#include "CCCallLambda.h"
#include "Core/MathHelper.h"

//----CCCallLambda-----

CCCallLambda::CCCallLambda()
	: _isfuncEndCalled()
{
}

void CCCallLambda::update(float time)
{
	if (_isfuncEndCalled)
		return;

	//if (isDone()) - это прием не работает с CCSequence
	//_funcEnd может вызваться несколько раз! из-за погрешности в Math::IsNumEquals
	if (Math::IsNumEquals(time, 1))  
	{
		_funcEnd();
		_funcEnd.Clear();
		_isfuncEndCalled = true;
	}
}

//----CCCallLambdaInstant-----

CCCallLambdaInstant::CCCallLambdaInstant()
{
}

void CCCallLambdaInstant::execute()
{
	_func();
	_func.Clear();
}

void CCCallLambdaInstant::update(float time)
{
	this->execute();
}


//----CCCallLambdaUpdate---

CCCallLambdaUpdate::CCCallLambdaUpdate()
	: _isDone(), _isFirstStepCompleted()
{
}

CCCallLambdaUpdate::~CCCallLambdaUpdate()
{
}

bool CCCallLambdaUpdate::isDone()
{
	return _isDone;
}

void CCCallLambdaUpdate::step(float dt)
{
	if (!_isFirstStepCompleted)  //пропускаем первый update
	{
		_isFirstStepCompleted = true;
		return;
	}

	_isDone = _funcUpdate();
	if (_isDone)
	{
		_funcEnd();
		_funcEnd.Clear();
	}
}

//------CCCallLambdaEasingUpdate-------

CCCallLambdaEasingUpdate::CCCallLambdaEasingUpdate()
	: _easingType(DracoGames::EasingCurveType::EasingCurveLinear), _easingBackOvershoot(0)
{
}

CCCallLambdaEasingUpdate* CCCallLambdaEasingUpdate::SetEasingType(DracoGames::EasingCurveType easingType, float backOvershoot) 
{ 
	_easingType = easingType;
    _easingBackOvershoot = backOvershoot;
	return this;
}

void CCCallLambdaEasingUpdate::update(float percent)
{
    const bool isHasOvershoot = !Math::IsZero(_easingBackOvershoot);
    
    if (isHasOvershoot)
        EasingCurves::push_overshoot(_easingBackOvershoot);
	float percentReal = EasingCurves::eval(percent, _easingType);
    if (isHasOvershoot)
        EasingCurves::pop_overshoot();
	float newValue = _valueFrom + (_valueTo - _valueFrom) * percentReal;
	_funcSetter(newValue);
}
