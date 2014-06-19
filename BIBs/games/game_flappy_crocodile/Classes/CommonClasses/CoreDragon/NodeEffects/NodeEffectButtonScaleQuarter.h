#pragma once
#ifndef NodeEffectButtonScaleQuarter_h
#define NodeEffectButtonScaleQuarter_h

#include "Core/CommonMacros.h"
#include "Dragon/effects/node_effect_button_scale.h"
#include "Dragon/utilities/ig_easing_curves.h"
using namespace DracoGames;


//TODO: все эффекты отнаследовать от CCAction!

//скейлить от 0 до 1, далее от 1 до 0, от 0 до -1, от -1 до 0
class NodeEffectButtonScaleQuarter : public NodeEffectButtonScale
{
public:
	NodeEffectButtonScaleQuarter(cocos2d::CCNode& nodeToEffect, const CCPoint& scalePhase1, 
		const CCPoint& scalePhase2, const float timeInSeconds);
	NodeEffectButtonScaleQuarter* clone() const;

private:
	void onFinish(void);
	void onUpdate(const float deltaTime);

	float GetQuarterPercentReal();
	float GetNewScaleQuarter(float initialScale, float scalePhase1, float scalePhase2);

private:
	const CCPoint _scalePhase1; //скейл (по x и по y) фазы1 (25% * t)
	const CCPoint _scalePhase2; //скейл фазы2 (75% * t)
};



class NodeEffectButtonScalePercent : public NodeEffectButtonScale
{
	public_ struct ScaleStruct
	{
		ScaleStruct() 
			: ScaleX(0), ScaleY(0), Percent(0), IsEasingInvert() 
		{
		}

		ScaleStruct(float scaleX, float scaleY, float percent, bool isEasingInvert)
			: ScaleX(scaleX), ScaleY(scaleY), Percent(percent), IsEasingInvert(isEasingInvert)
		{
		}

		float ScaleX;
		float ScaleY;
		float Percent;//0, 25, ...
		bool IsEasingInvert;
	};

	private_ std::vector<ScaleStruct> _initialScales;

	public_ NodeEffectButtonScalePercent(cocos2d::CCNode& nodeToEffect, float timeInSeconds, EasingCurveType easingType);
	public_ NodeEffectButtonScalePercent* clone() const;

	public_ void AddScale(float scaleX, float scaleY, float percent, bool isEasingInvert);
	public_ void AddScale(const ScaleStruct& scaleStruct);
	public_ void SetScales(const std::vector<ScaleStruct>& scales);

	private_ void onStart(void);
	private_ void onFinish(void);
	public_ virtual void onUpdate(const float deltaTime);

	private_ int GetScaleIndexByPercent(float percent);
};


#endif
