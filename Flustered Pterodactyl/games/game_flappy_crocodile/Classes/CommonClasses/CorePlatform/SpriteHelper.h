#pragma once
#ifndef SpriteHelper_h
#define SpriteHelper_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include "CorePlatform/GeometryHelper.h"
#include "CorePlatform/CommonProperties.h"
using namespace cocos2d;
#include "Core/PropertyDefiner.h"
#include <ccTypes.h>
#include <cocoa/CCObject.h>
#include <base_nodes/CCNode.h>
#include <sprite_nodes/CCSprite.h>
#include <support/CCPointExtension.h>
namespace cocos2d 
{
	class CCSpriteFrame; class CCTexture2D; class CCSpriteBatchNode;
	class CCBlendProtocol; class CCMenu; class CCMenuItemLabel; class CCMenuItemSprite;
}


class CCSpriteWrapper;

namespace SpriteHelper
{	
	CCSpriteFrame* LoadFrame(const string& fileName);
	CCTexture2D* LoadAtlasTexture(const string& fileimage, const string& plist, bool noNeedHDSuffix = false, const string& framePrefix = "");
	CCTexture2D* LoadTexture(const string& fileimage, bool noNeedHDSuffix = false);
    string GetTexturePath(const string& fileimage);
	void SetAntiAliasing(CCTexture2D* texture, bool isTrue);
	void SetTexturePropertiesByRetina(CCTexture2D* texture);
    
	void SetBlendFuncForPremultiplied(CCSpriteBatchNode* node);
	void SetIsOpacityModifyForPremultiplied(CCSprite* sprite);
	CCTexture2D* LoadTextureAsPremultipliedPng(const char* fileimage);
	void SetAnchorPointInPixels(CCNode* node, const CCPoint& anchorPointInPoints);
	CCPoint GetAnchorPointInPixels(CCNode* node);
	Rect GetSpriteRect(CCNode* node);
	void DrawFilledPolygon(const CCPoint* poli, int numberOfPoints, GLenum drawingMode);
	ccBlendFunc GetBlendFunc(GLenum src, GLenum dst);
	void SetSpriteRenderingWithoutAlpha(CCBlendProtocol* sprite);

	string GetFontPath();
	CCMenuItemLabel* CreateLabel(CCObject* target, const char* text, const CCPoint& anchorPoint, SEL_MenuHandler selector);
	void FillSpriteFramesFromAtlas(CCSpriteFrame** result, int totalFramesCount, CCTexture2D* texture, CCSize frameSize, int rowCount, int columnCount);

	CCSpriteWrapper* CreateEmptySprite(CCNode* parent, int z);
	CCSpriteWrapper* CreateSpriteWithTextureName(CCNode* parent, int z, const string& textureName, const CCPoint& anchorPoint = ccp(0.5f, 0.5f));
	CCSpriteWrapper* CreateSpriteWithTexture(CCNode* parent, int z, CCTexture2D* texture, const CCPoint& anchorPoint = ccp(0.5f, 0.5f));
	CCSpriteWrapper* CreateSpriteWithFrame(CCNode* parent, int z, CCSpriteFrame* frame, const CCPoint& anchorPoint = ccp(0.5f, 0.5f));
	CCSpriteWrapper* CreateSpriteWithFrameName(CCNode* parent, int z, const string& frameName, const CCPoint& anchorPoint = ccp(0.5f, 0.5f));
	CCSpriteFrame* GetSpriteFrameFromTextureName(const string& textureName);
	CCSpriteFrame* GetSpriteFrameFromTexture(CCTexture2D* texture);

	CCNode* CreateEmptyNode(CCNode* parent, int z);

	CCMenuItemSprite* AddMenuItemToMenu(CCMenu* menu, const string& frameNonActiveName, const string& frameActiveName, CCObject* target, SEL_MenuHandler selector);
	CCMenu* CreateMenu(CCNode* parent, int z);

	void SetNodeSdScale(CCNode* node);
	CCSize GetTextureContentSize(CCTexture2D* texture);
	void SetSpriteTextureRect(CCSprite* sprite, const CCRect& rect);
	CCSize GetContentSize(CCNode* node);
	void SetContentSize(CCNode* node, const CCSize& size);

	void SetBlendAdd(CCSprite* sprite);
	void SetBlendMultiply(CCSprite* sprite);
}

static ccBlendFunc BLEND(GLenum src, GLenum dst) { return SpriteHelper::GetBlendFunc(src, dst); }


namespace SpriteHelper
{
	template<typename TNode>
	TNode* CreateNode(CCNode* parent, int z)
	{
		auto node = new TNode();
		parent->addChild(node, z);
		node->release();
		return node;
	}

	template<typename TNode>
	TNode* AddNodeAndRelease(CCNode* parent, int z, TNode* nodeToAdd)
	{
		parent->addChild(nodeToAdd, z);
		nodeToAdd->release();
		return nodeToAdd;
	}	
}


class CCNodeWrapperWrapper : NonCopyable
{
	protected_ CCNode* _node;
	protected_ bool _isNeedToRelease;
	public_ CCNodeWrapperWrapper(CCNode* node) : _node(node), _isNeedToRelease(false) 
	{
		Init();
	}

	protected_ CCNodeWrapperWrapper() : _node(NULL), _isNeedToRelease(false) 
	{
	}

	protected_ void Init()
	{
		Visible.Init(_node, &CCNode::isVisible, &CCNode::setVisible);
	}

	public_ ~CCNodeWrapperWrapper() 
	{ 
		if (_isNeedToRelease)
			_node->release();
	}

	public_ PropertyDefiner<bool, CCNode> Visible;
	public_ float X() { return _node->getPosition().x; }
	public_ float Y() { return _node->getPosition().y; }
	public_ void SetX(float x) { _node->setPosition(ccp(x, _node->getPosition().y)); }
	public_ void SetY(float y) { _node->setPosition(ccp(_node->getPosition().x, y)); }

	public_ float FrameLeft();
	public_ float FrameBottom();

	void SetAnchorPointInPixels(const CCPoint& anchorPointInPoints);
	CCPoint ConvertToNodeSpace(const CCPoint& worldPoint);
	CCPoint ConvertTouchToNodeSpace(CCTouch *touch);
	CCPoint ConvertToWorldSpace(const CCPoint& nodePoint);
};


template<typename T = CCNode>
class CCNodeWrapper : public T, public CCNodeWrapperWrapper
{
	public_ CCNodeWrapper()
	{
		CCNodeWrapperWrapper::_node = this;
		CCNodeWrapperWrapper::Init();
	}
};


class CCSpriteWrapperWrapper : public CCNodeWrapperWrapper
{
	protected_ CCSprite* _sprite;

	public_ CCSpriteWrapperWrapper(CCSprite* sprite) : CCNodeWrapperWrapper(sprite), _sprite(sprite)
	{
	}

	protected_ CCSpriteWrapperWrapper() : CCNodeWrapperWrapper(), _sprite(NULL)
	{ 
	}
	
	public_ float Width();
	public_ float Height();

	public_ float FrameRight();
	public_ float FrameTop();
	public_ Rect GetRectUnscaled();
	public_ CCPoint PointCenter();

	public_ void SetSpriteRenderingWithoutAlpha() { SpriteHelper::SetSpriteRenderingWithoutAlpha(_sprite); }
	public_ Rect GetRect() { return SpriteHelper::GetSpriteRect(_sprite); }
	public_ void SetBlendAdd() { SpriteHelper::SetBlendAdd(_sprite); }
	public_ void SetBlendMultiply() { SpriteHelper::SetBlendMultiply(_sprite); }
	public_ void SetOpacityInPercent(float percent) { _sprite->setOpacity((GLubyte)(percent * 255)); }
	public_ float GetOpacityInPercent() { return ((float)_sprite->getOpacity())/255; }
	public_ void AddRotation(float deltaRotation) { _node->setRotation(_node->getRotation() + deltaRotation); }

	public_ void SetScaleByScaleX(float scaleXAbsolute);
	public_ void SetScaleByScaleAbsolute(const CCSize& scaleSizeAbsolute);
};


//hoho, множественное наследование-таки пригодилось)
class CCSpriteWrapper : public CCSprite, public CCSpriteWrapperWrapper
{
	private_ int _id;

	public_ CCSpriteWrapper()
	{
		static int staticId = 0;
		_id = ++staticId;

		_node = _sprite = this;
		CCSpriteWrapperWrapper::Init();
	}
};

#endif
