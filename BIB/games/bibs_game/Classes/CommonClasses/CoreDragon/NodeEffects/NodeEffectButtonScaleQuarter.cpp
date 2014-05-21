#include "NodeEffectButtonScaleQuarter.h"
#include "CorePlatform/LogHelper.h"

//---------------NodeEffectButtonScaleQuarter-------------

NodeEffectButtonScaleQuarter::NodeEffectButtonScaleQuarter(cocos2d::CCNode& nodeToEffect, 
	const CCPoint& scalePhase1, 
	const CCPoint& scalePhase2, const float timeInSeconds) :
	NodeEffectButtonScale(nodeToEffect, 0, 0, timeInSeconds),
	_scalePhase1(scalePhase1), _scalePhase2(scalePhase2)
{
}

NodeEffectButtonScaleQuarter* NodeEffectButtonScaleQuarter::clone() const
{
	auto effect = new NodeEffectButtonScaleQuarter(m_node, _scalePhase1, _scalePhase2, m_totalTime);
	effect->SetEasingType(_easingType, _backOvershoot);
	effect->SetLoopCount(GetLoopCount());
	return effect;
}

void NodeEffectButtonScaleQuarter::onFinish(void)
{
	m_node.setScaleX(_initialScale.x);
	m_node.setScaleY(_initialScale.y);

	INodeEffect::onFinish();
}

void NodeEffectButtonScaleQuarter::onUpdate(const float deltaTime)
{
	CalculateTotalTime(deltaTime);

	m_node.setScaleX(GetNewScaleQuarter(_initialScale.x, _scalePhase1.x, _scalePhase2.x));
	m_node.setScaleY(GetNewScaleQuarter(_initialScale.y, _scalePhase1.y, _scalePhase2.y));
}

float NodeEffectButtonScaleQuarter::GetQuarterPercentReal()
{
	float perc = 0;
	if (_percentage < 0.25f) perc = _percentage * 4;
	else if (_percentage < 0.5f) perc = 2 - _percentage * 4;
	else if (_percentage < 0.75f) perc = (_percentage - 0.5f) * 4;
	else perc = 2 - (_percentage - 0.5f) * 4;

	if (_backOvershoot != 0.0f) EasingCurves::push_overshoot(_backOvershoot);
	float percentReal = EasingCurves::eval(perc, _easingType);
	if (_backOvershoot != 0.0f) EasingCurves::pop_overshoot();

	return percentReal;
}

float NodeEffectButtonScaleQuarter::GetNewScaleQuarter(float initialScale, float scalePhase1, float scalePhase2)
{
	float percentReal = GetQuarterPercentReal();

	float newScale = 0;
	if (_percentage < 0.5f)
		newScale = initialScale + percentReal * (scalePhase1 - initialScale);
	else
		newScale = initialScale + percentReal * (scalePhase2 - initialScale);

	return newScale;
}



//--------------NodeEffectButtonScalePercent-----------------

NodeEffectButtonScalePercent::NodeEffectButtonScalePercent(cocos2d::CCNode& nodeToEffect, 
	float timeInSeconds, EasingCurveType easingType)
	: NodeEffectButtonScale(nodeToEffect, 0, 0, timeInSeconds)
{
	_easingType = easingType;
}

NodeEffectButtonScalePercent* NodeEffectButtonScalePercent::clone() const
{
	auto effect = new NodeEffectButtonScalePercent(m_node, m_totalTime, _easingType);
	effect->_initialScales.assign(_initialScales.begin(), _initialScales.end());
	effect->SetLoopCount(GetLoopCount());
	return effect;
}

void NodeEffectButtonScalePercent::AddScale(float scaleX, float scaleY, float percent, bool isEasingInvert)
{
	_initialScales.push_back(ScaleStruct(scaleX, scaleY, percent, isEasingInvert));
}

void NodeEffectButtonScalePercent::AddScale(const ScaleStruct& scaleStruct)
{
	_initialScales.push_back(scaleStruct);
}

void NodeEffectButtonScalePercent::SetScales(const std::vector<ScaleStruct>& scales)
{
	_initialScales = scales;
}

void NodeEffectButtonScalePercent::onStart(void)
{
	NodeEffectButtonScale::onStart();

	_percentage = 0;
}

void NodeEffectButtonScalePercent::onFinish(void)
{
	m_node.setScaleX(_initialScales.back().ScaleX);
	m_node.setScaleY(_initialScales.back().ScaleY);

	INodeEffect::onFinish();
}

void NodeEffectButtonScalePercent::onUpdate(const float deltaTime)
{
	CalculateTotalTime(deltaTime);

	float percentReal = _percentage;
	//float percentReal = EasingCurves::eval(_percentage, _easingType);
	int index = GetScaleIndexByPercent(percentReal);		

	auto prevScale = _initialScales[index];
	auto nextScale = _initialScales[index + 1];

	float localPercent = Math::InverseLerp(prevScale.Percent, nextScale.Percent, percentReal);
	if (nextScale.IsEasingInvert)
		localPercent = 1 - localPercent;
	localPercent = EasingCurves::eval(localPercent, _easingType);

	if (nextScale.IsEasingInvert)
		std::swap(prevScale, nextScale);

	float newScaleX = Math::Lerp(prevScale.ScaleX, nextScale.ScaleX, localPercent);
	float newScaleY = Math::Lerp(prevScale.ScaleY, nextScale.ScaleY, localPercent);

	m_node.setScaleX(newScaleX);
	m_node.setScaleY(newScaleY);
}

int NodeEffectButtonScalePercent::GetScaleIndexByPercent(float percent)
{
	for (int i = 0, n = _initialScales.size() - 1; i < n; ++i)
	{
		if (Math::IsBetweenExcludeLast(percent, _initialScales[i].Percent, 
			_initialScales[i+1].Percent))
		{
			return i;
		}
	}

	return _initialScales.size() - 2;
}