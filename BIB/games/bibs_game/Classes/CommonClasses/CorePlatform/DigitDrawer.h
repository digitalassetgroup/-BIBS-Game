#pragma once
#ifndef DigitDrawer_h
#define DigitDrawer_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
using namespace cocos2d;
#include "SpriteHelper.h"

class DigitDrawer
{
	private_ CCSpriteFrame* _frameDigits[10];
	private_ CCSpriteWrapper* _spriteLevel[10];
	private_ int _countOfDigits;

	public_ DigitDrawer(int countOfDigits)
		: _countOfDigits(countOfDigits)
	{
		CollectionHelper::ClearArray(_frameDigits);
		CollectionHelper::ClearArray(_spriteLevel);
	}

	public_ void SetVisible(bool isVisible)
	{
		for (int i = 0; i < _countOfDigits; ++i)
			_spriteLevel[i]->setVisible(isVisible);
	}

	public_ void SetOpacityInPercent(float opacity)
	{
		for (int i = 0; i < _countOfDigits; ++i)
			_spriteLevel[i]->SetOpacityInPercent(opacity);
	}

	public_ void InitFramesFromFormatString(const string& formatString)
	{
		for (int i = 0; i <= 9; ++i)
			_frameDigits[i] = SpriteHelper::LoadFrame(StringHelper::format(formatString.c_str(), i));
	}

	public_ void CreateSprites(CCNode* parentNode, int z, const CCPoint& position, float digitWidth,
		const CCPoint& anchorPoint)
	{
		for (int i = 0; i < _countOfDigits; ++i)
		{
			_spriteLevel[i] = SpriteHelper::CreateSpriteWithFrame(parentNode, z, _frameDigits[0]); //here I need to retain, I think
			_spriteLevel[i]->setAnchorPoint(anchorPoint);

			_spriteLevel[i]->setPosition(ccp(position.x + digitWidth*(_countOfDigits - 1 - i), 
				position.y));
			SpriteHelper::SetNodeSdScale(_spriteLevel[i]);
		}
	}

	public_ void SetNumber(int number)
	{
		for (int i = 0; i < _countOfDigits; ++i)
		{
			int digit = Math::GetDigit(number, i);
			_spriteLevel[i]->setDisplayFrame(_frameDigits[digit]);
		}
	}
};

#endif
