#ifndef NodeEffectCustomValue_h
#define NodeEffectCustomValue_h

#include "Core/CommonMacros.h"
#include "NodeEffectWithEasingBase.h"
using namespace DracoGames;


template<typename TValue, typename TSetter>
class NodeEffectCustomValue : public NodeEffectWithEasingBase
{
	private_ TValue _valueFrom;
	private_ TValue _valueTo;
	private_ TSetter _funcSetter;

	public_ NodeEffectCustomValue(CCNode& nodeToEffect, float timeInSeconds, 
		const TValue& valueFrom, const TValue& valueTo, const TSetter& funcSetter)
		: NodeEffectWithEasingBase(nodeToEffect, timeInSeconds), 
		_valueFrom(valueFrom),
		_valueTo(valueTo), 
		_funcSetter(funcSetter)
	{
	}

	virtual INodeEffect* clone() const 
	{ 
		auto effect = new NodeEffectCustomValue<TValue, TSetter>(m_node, _totalTime, 
			_valueFrom, _valueTo, _funcSetter);
		effect->SetLoopCount(GetLoopCount());
		effect->SetEasingType(_easingType);
		return effect;
	}

	virtual void onUpdate(const float deltaTime)
	{
		NodeEffectWithEasingBase::onUpdate(deltaTime);

		float percentReal = EasingCurves::eval(_percentage, _easingType);
		TValue newValue = Math::Lerp(_valueFrom, _valueTo, percentReal);
		_funcSetter(newValue);
	}
};


template<typename TValue, typename TSetter>
NodeEffectCustomValue<TValue, TSetter>* CreateNodeEffectCustomValue(
	CCNode* nodeToEffect, float timeInSeconds, const TValue& valueFrom, const TValue& valueTo, const TSetter& funcSetter)
{
	return new NodeEffectCustomValue<TValue, TSetter>(
		*nodeToEffect, timeInSeconds, valueFrom, valueTo, funcSetter);
}

#endif
