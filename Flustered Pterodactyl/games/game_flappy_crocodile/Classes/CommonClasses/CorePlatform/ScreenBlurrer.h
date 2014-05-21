#pragma once
#ifndef ScreenBlurrer_h
#define ScreenBlurrer_h

#include "Core/CommonMacros.h"
#include "Core/ObserverEvent.h"

namespace cocos2d { class CCNode; class CCRenderTexture; }
class CCSpriteWrapper;

using namespace cocos2d;


class ScreenBlurrer
{
	public_ ObserverEvent<CCNode*> OnRenderNode;

	private_ CCNode* _nodeToRender;
	private_ const float _precision; //adjust the blur precision (1 : accurate ~ 10 : abstract)
	private_ const float _maxAmount; //adjust the blur amount
	private_ const float _dAmount;
	private_ float _amount; //adjust the blur amount
	private_ float _width;
	private_ float _height;
	private_ bool _isSpriteBlurTransparent;

	private_ CCRenderTexture* _blur1X;
	private_ CCRenderTexture* _blur2X;
	private_ CCSpriteWrapper* _spriteBlur;	
	private_ bool _isGoBackward;

	public_ ScreenBlurrer(CCNode* nodeToRender, float precision, float maxAmount, float dAmount,
		bool isSpriteBlurTransparent = false);
	public_ ~ScreenBlurrer();

	private_ void Stop();

	private_ void CreateSpriteBlur();
	public_ void AddSpriteBlurToNode(CCNode* parent, int z);
	
	//сделать текстуре блюр n раз
	public_ void UpdateRenderTexture(int n);

	//true - go backward, false - go forward
	public_ void Init(bool isGoBackward);
	public_ bool Update();

	private_ void BeginAnimation();
	private_ void UpdateSpriteBlur();
	private_ void UpdateRenderTexture();
	private_ void UpdateSpriteBlurOpacity();
	private_ void DrawSpriteBlur();
};


#endif
