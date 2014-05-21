#include "ScreenBlurrer.h"
#include "SpriteHelper.h"
#include "InputHelper.h"
#include "LogHelper.h"
#include <cocos2d.h>


ScreenBlurrer::ScreenBlurrer(CCNode* nodeToRender, float precision, float maxAmount, float dAmount,
	bool isSpriteBlurTransparent)
	: _nodeToRender(nodeToRender), _precision(precision), _maxAmount(maxAmount), _dAmount(dAmount), 
		_isSpriteBlurTransparent(isSpriteBlurTransparent), _amount(), _blur1X(), _blur2X(), _spriteBlur()
{
	auto winSize = InputHelper::GetUnscaledWindowSize();
	_width=winSize.width/_precision;
	_height=winSize.height/_precision;
}

ScreenBlurrer::~ScreenBlurrer()
{
	Stop();
	if (_spriteBlur == NULL)
		return;

	if (_spriteBlur->getParent() != NULL)
		_spriteBlur->removeFromParentAndCleanup(false);
	_spriteBlur->release();
	_spriteBlur = NULL;
}

void ScreenBlurrer::Stop()
{
	if (_blur1X == NULL)
		return;

	_blur1X->release();
	_blur1X = NULL;
	_blur2X->release();
	_blur2X = NULL;
}

void ScreenBlurrer::CreateSpriteBlur()
{
	if (_spriteBlur != NULL)
		return;

	_spriteBlur = new CCSpriteWrapper();
	_spriteBlur->init();
	_spriteBlur->setAnchorPoint(ccp(0.5f, 0.5f));
}

void ScreenBlurrer::AddSpriteBlurToNode(CCNode* parent, int z)
{
	CreateSpriteBlur();
	parent->addChild(_spriteBlur, z);
	//_spriteBlur->SetOpacityInPercent(Math::Min(1.0f, _amount/_precision));
	_spriteBlur->setFlipY(true);
	_spriteBlur->setScale(_precision);
	//_spriteBlur->setPosition(ccp(0, 0));

	auto winSize = InputHelper::GetUnscaledWindowSize();
	_spriteBlur->setPosition(ccp(winSize.width/2, winSize.height/2));
}
	
//сделать текстуре блюр n раз
void ScreenBlurrer::UpdateRenderTexture(int n)
{
	for (int i = 0; i < n; ++i)
	{
		UpdateRenderTexture();
	}

	UpdateSpriteBlur();
	_amount = _maxAmount + 1;

	if (_isSpriteBlurTransparent)
		_spriteBlur->SetOpacityInPercent(Math::Min(1.0f, ((float)n)/_precision));
}

//true - go backward, false - go forward
void ScreenBlurrer::Init(bool isGoBackward)
{
	_isGoBackward = isGoBackward;
	if (_isGoBackward && _amount == 0)
		_amount = _maxAmount;
	else if (!_isGoBackward)
		_amount = 0;

	if (_blur1X == NULL)
	{
		_blur1X = new CCRenderTexture();
		_blur1X->initWithWidthAndHeight(_width/2, _height/2, 
			CCTexture2DPixelFormat::kCCTexture2DPixelFormat_RGBA8888);

		_blur2X = new CCRenderTexture();
		_blur2X->initWithWidthAndHeight(_width, _height, 
			CCTexture2DPixelFormat::kCCTexture2DPixelFormat_RGBA8888);
	}

	if (!_isGoBackward)
		BeginAnimation();
}

bool ScreenBlurrer::Update()
{
	if (!_isGoBackward && _amount > _maxAmount) {
		Stop(); return false;
	}
	else if (_isGoBackward && _amount < 0) {
		Stop(); return false;
	}

	float oldAmount = _amount;
	_amount += (_isGoBackward ? -1 : 1) * _dAmount;
	UpdateSpriteBlurOpacity();
	if (_spriteBlur->getTexture() == NULL || Math::TruncF(oldAmount) != Math::TruncF(_amount))
		DrawSpriteBlur();
	return true;
}

void ScreenBlurrer::BeginAnimation()
{
	auto originalAnchorPoint = _nodeToRender->getAnchorPoint();
	float originalScale = _nodeToRender->getScale();
	CCPoint originalPosition = _nodeToRender->getPosition();
	_blur2X->begin();
	{
		//NOTE: мега-костыль!
		_nodeToRender->setPosition(ccp(0 - ccp(0, 0).x/_precision, 
			0 - ccp(0, 0).y/_precision));
		_nodeToRender->setScale(1/_precision);
		_nodeToRender->setAnchorPoint(ccp(0, 0));
		if (!OnRenderNode.IsEmpty())
			OnRenderNode.Notify(_nodeToRender);
		else
			_nodeToRender->visit();
		_nodeToRender->setAnchorPoint(originalAnchorPoint);
		_nodeToRender->setScale(originalScale);
		_nodeToRender->setPosition(originalPosition);
	}
	_blur2X->RENDERTEXTURE_END;
	//_blur2X->saveBuffer(eImageFormat::kCCImageFormatPNG, "blur2x.png");

	_blur2X->getSprite()->getTexture()->setAntiAliasTexParameters();
	UpdateSpriteBlur();
}

void ScreenBlurrer::UpdateSpriteBlur()
{
	CreateSpriteBlur();
	_spriteBlur->setTexture(_blur2X->getSprite()->getTexture());		
	_spriteBlur->setTextureRect(CCRect(0, 0, 
		_spriteBlur->getTexture()->getContentSize().width, _spriteBlur->getTexture()->getContentSize().height));
	//_spriteBlur->getTexture()->setAliasTexParameters();
	if (!_isSpriteBlurTransparent)
		_spriteBlur->SetSpriteRenderingWithoutAlpha();

	UpdateSpriteBlurOpacity();
}

void ScreenBlurrer::UpdateRenderTexture()
{
	_blur1X->begin();
	{
		_blur2X->setScale(0.5f);
		auto anchorPoint = _blur2X->getSprite()->getAnchorPoint();
		_blur2X->getSprite()->setAnchorPoint(ccp(0, 1));
		_blur2X->getSprite()->getTexture()->setAntiAliasTexParameters();
		//_blur2X->getSprite()->getTexture()->setAliasTexParameters();
		SpriteHelper::SetSpriteRenderingWithoutAlpha(_blur2X->getSprite());
		{
			_blur2X->visit();
		}
		_blur2X->getSprite()->setAnchorPoint(anchorPoint);
		_blur2X->setScale(1);
	}
	_blur1X->RENDERTEXTURE_END;
	//_blur1X->saveBuffer(eImageFormat::kCCImageFormatPNG, StringHelper::format("blur1x-%d.png", i).c_str());

	_blur2X->begin();
	{
		_blur1X->setScale(2);
		auto anchorPoint = _blur1X->getSprite()->getAnchorPoint();
		_blur1X->getSprite()->setAnchorPoint(ccp(0, 1));
		_blur1X->getSprite()->getTexture()->setAntiAliasTexParameters();
		SpriteHelper::SetSpriteRenderingWithoutAlpha(_blur1X->getSprite());
			_blur1X->visit();
		_blur1X->getSprite()->setAnchorPoint(anchorPoint);
		_blur1X->setScale(1);
	}
	_blur2X->RENDERTEXTURE_END;
	//_blur2X->saveBuffer(eImageFormat::kCCImageFormatPNG, StringHelper::format("blur2x-%d.png", (int)_amount).c_str());
}

void ScreenBlurrer::UpdateSpriteBlurOpacity()
{
	if (_isSpriteBlurTransparent)
		_spriteBlur->SetOpacityInPercent(Math::Clamp(_amount/(_precision + 1), 0.0f, 1.0f));
	//MY_LOG("opacity = " << _spriteBlur->GetOpacityInPercent() << "; time = " << TotalTimeCounter::GetTotalTime());
}

void ScreenBlurrer::DrawSpriteBlur()
{
	UpdateSpriteBlurOpacity();

	//MY_LOG("amount = " << _amount);
	if (!_isGoBackward)
	{
		UpdateRenderTexture();
	}
	else
	{
		BeginAnimation();
		for (int i = 0; i < _amount; ++i)
		{
			UpdateRenderTexture();
		}
	}
}