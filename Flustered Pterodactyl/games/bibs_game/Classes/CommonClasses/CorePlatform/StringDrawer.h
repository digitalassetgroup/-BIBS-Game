#pragma once
#ifndef StringDrawer_h
#define StringDrawer_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include <cocos2d.h>
using namespace cocos2d;
#include "SpriteHelper.h"
#include "LogHelper.h"
#include <map>
using std::map;
using std::pair;

class StringDrawer
{
	private_ map<char, CCSpriteFrame*> _framesLetter;
	private_ vector<CCSpriteWrapper*> _sprites;
	private_ CCSpriteBatchNode* _batchNode; //весь текст пихаем в batchNode
	
	protected_ vector<pair<string, char> > _mapFromFileNameToCharName;
	protected_ int _deltaLetterInterval;
	private_ CCPoint _startPosition;
	public_ void SetStartPositionX(float startPositionX) { _startPosition.x = startPositionX; }

	private_ CCPoint _letterAnchorPoint;

	public_ StringDrawer() : _batchNode(), _deltaLetterInterval(-3)
	{ 
	}

	public_ void SetOpacityInPercent(float opacity)
	{
		for_each(auto sprite, _sprites)
			sprite->SetOpacityInPercent(opacity);
	}

	protected_ void InitFramesFromFormatString(const string& formatString)
	{
		for_each_ref(auto& pair, _mapFromFileNameToCharName)
		{
			_framesLetter[pair.second] = SpriteHelper::LoadFrame(StringHelper::format(formatString.c_str(), pair.first.c_str()));
		}
		for_each_ref_end
	}

	public_ void InitBatchNode(CCNode* parentNode, int z, CCTexture2D* textureFont, 
		const CCPoint& startPosition, const CCPoint& letterAnchorPoint)
	{
		_batchNode = new CCSpriteBatchNode();
		_batchNode->initWithTexture(textureFont, 100);
		parentNode->addChild(_batchNode, z);
		_batchNode->release();

		_startPosition = startPosition;
		_letterAnchorPoint = letterAnchorPoint;
	}

	protected_ virtual int GetDeltaLetterInterval(char letter)
	{
		return _deltaLetterInterval;
	}

	private_ CCSpriteFrame* GetLetterFrame(char letter, int& letterWidth)
	{
		if (letter == ' ')
		{
			letterWidth = 18;
			return NULL;
		}

		auto letterFrame = _framesLetter.find(letter);
		if (letterFrame == _framesLetter.end())
		{
			MY_LOG_ERROR_F("Couldnt find letter '%c'", letter);
			letterWidth = 0;
			return NULL;
		}

		letterWidth = letterFrame->second->getRectInPixels().size.width + GetDeltaLetterInterval(letter);
		return letterFrame->second;
	}

	//вернуть ширину строки
	public_ int GetWidthOfString(const string& str)
	{
		int width = 0;
		for_each_ref(char letter, str)
		{
			int letterWidth = 0;
            GetLetterFrame(letter, letterWidth);
			width += letterWidth;
		}
		for_each_ref_end

		return width;
	}

	public_ void SetString(const string& str)
	{
		//очищаем все спрайты и релизим их
		_batchNode->removeAllChildrenWithCleanup(false);
		_sprites.clear();

		auto positionX = _startPosition.x;
		for_each_ref(char letter, str)
		{
			int letterWidth = 0;
			auto frame = GetLetterFrame(letter, letterWidth);

			if (frame != NULL)
			{
				auto sprite = new CCSpriteWrapper();
				sprite->initWithSpriteFrame(frame);
				_batchNode->addChild(sprite, 0);
				sprite->release();
				sprite->setAnchorPoint(_letterAnchorPoint);
				sprite->setPosition(ccp(positionX, _startPosition.y));
				_sprites.push_back(sprite);
			}
			
			positionX += letterWidth;
		}
		for_each_ref_end
	}
};

#endif
