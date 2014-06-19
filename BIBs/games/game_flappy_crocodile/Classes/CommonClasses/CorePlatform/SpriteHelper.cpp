#include "SpriteHelper.h"
#include "CCGL.h"
#include "CorePlatform/CommonProperties.h"
#include "Core/StringHelper.h"
#include "CorePlatform/InputHelper.h"
#include "LogHelper.h"
#include <utility>
#include <cocos2d.h>

CCNode* SpriteHelper::CreateEmptyNode(CCNode* parent, int z)
{
	auto node = new CCNode();
	parent->addChild(node, z);
	node->release();
	return node;
}

string SpriteHelper::GetTexturePath(const string& fileimage)
{
    string result(fileimage);
	//NOTE: temp comment. out hd-images without "hd"-suffix yet
 //if (CommonProperties::IsLoadHDImages())
 //   {      
 //       StringHelper::Replace(result, ".png", "-hd.png");
 //       StringHelper::Replace(result, ".plist", "-hd.plist");
 //   }
    return result;
}

CCSpriteFrame* SpriteHelper::LoadFrame(const string& fileName)
{
	auto result = CCSpriteFrameCache::sharedSpriteFrameCache()
		->spriteFrameByName(fileName.c_str());

	MY_ASSERT(result != NULL, "SpriteFrame not found");
	return result;
}

//noNeedHDSuffix - не нужно добавлять hd-суффикс
CCTexture2D* SpriteHelper::LoadAtlasTexture(const string& fileimage, const string& plist, bool noNeedHDSuffix, const string& framePrefix)
{
	auto texture = CCTextureCache::sharedTextureCache()->addImage(noNeedHDSuffix ? fileimage.c_str() : GetTexturePath(fileimage).c_str());
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(noNeedHDSuffix ? plist.c_str() : GetTexturePath(plist).c_str(), 
		texture, framePrefix);
	MY_ASSERT(texture != NULL, MAKE_STRING("No file " << fileimage));
	//MY_LOG(CCFileUtils::fullPathForFilename(fileimage));
	return texture;
}

CCTexture2D* SpriteHelper::LoadTexture(const string& fileimage, bool noNeedHDSuffix)
{
	auto texture = CCTextureCache::sharedTextureCache()->addImage(
		noNeedHDSuffix ? fileimage.c_str() : SpriteHelper::GetTexturePath(fileimage).c_str());
	MY_ASSERT(texture != NULL, MAKE_STRING("No file " << fileimage));
	SpriteHelper::SetTexturePropertiesByRetina(texture);
	return texture;
}

//установить спрайту антиалясинг (если true) и алясинг (если false)
//в зависимости от GetScreenScale
void SpriteHelper::SetAntiAliasing(CCTexture2D* texture, bool isTrue)
{
	if (CommonProperties::GetScreenScale() != 1.0f) //если есть скейл, то оставляем антиалясинг
		return;

	isTrue ? texture->setAntiAliasTexParameters()
		: texture->setAliasTexParameters();
}

//установить свойства текстуры в зависимости от того, есть ли ретина или нет
void SpriteHelper::SetTexturePropertiesByRetina(CCTexture2D* texture)
{
	texture->setAntiAliasTexParameters();

	/*
	if (CommonProperties::GetScreenScale() != 1.0f) //для андроидов придется фисе скейлить
	{
		texture->setAntiAliasTexParameters();

		//if (CommonProperties::GetScreenScale() < 1.0f)
		//{
		//	MY_LOG("Generate mip map");			
		//	texture->generateMipmap();
		//	ccTexParams texParams =  { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
		//	texture->setTexParameters(&texParams);
		//}

		return;
	}

	if (!CommonProperties::IsRetina())
	{
		texture->setAliasTexParameters();

		//texture->generateMipmap();
		//ccTexParams texParams =  { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
		//texture->setTexParameters(&texParams);
	}
	else
	{
		//texture->setAntiAliasTexParameters();
		texture->setAliasTexParameters();
	}
	*/
}

Rect SpriteHelper::GetSpriteRect(CCNode* node)
{
	auto anchorPoint = SpriteHelper::GetAnchorPointInPixels(node);
	auto position = ccpSub(node->getPosition(), anchorPoint);
	const auto& size = SpriteHelper::GetContentSize(node);

	return Rect(position.x, position.y, size.width, size.height);
	//auto& rectInPoints = sprite->getTextureRect();
	//return Rect(position.x, position.y, rectInPoints.size.width, rectInPoints.size.height);
}

//wrapper for sd-images support
CCSize SpriteHelper::GetTextureContentSize(CCTexture2D* texture)
{
	auto size = texture->getContentSize();
	float scale = !CommonProperties::IsLoadHDImages() ? 2 : 1;
	return CCSize(size.width * scale, size.height * scale);
}

//wrapper for sd-images support
void SpriteHelper::SetSpriteTextureRect(CCSprite* sprite, const CCRect& rect)
{
	float scale = !CommonProperties::IsLoadHDImages() ? 2 : 1;
	CCRect sdRect(rect.origin.x / scale, rect.origin.y / scale, 
		rect.size.width / scale, rect.size.height / scale);

	sprite->setTextureRect(sdRect);
}

CCPoint SpriteHelper::GetAnchorPointInPixels(CCNode* node)
{
	float scale = !CommonProperties::IsLoadHDImages() ? 2 : 1;
	CCPoint result = node->getAnchorPointInPoints();
	result = ccp(result.x * scale, result.y * scale);
	return result;
}

void SpriteHelper::SetAnchorPointInPixels(CCNode* node, const CCPoint& anchorPointInPoints)
{
	auto contentSize = SpriteHelper::GetContentSize(node);

	auto anchorPoint = ccp(anchorPointInPoints.x/contentSize.width,
		anchorPointInPoints.y/contentSize.height);
	node->setAnchorPoint(anchorPoint);
}

CCSize SpriteHelper::GetContentSize(CCNode* node)
{
	auto size = node->getContentSize();
	float scaleSd = !CommonProperties::IsLoadHDImages() ? 2 : 1;

	return CCSize(size.width * scaleSd, size.height * scaleSd);
}

void SpriteHelper::SetContentSize(CCNode* node, const CCSize& size)
{
	float scaleSd = !CommonProperties::IsLoadHDImages() ? 2 : 1;
	auto newSize = CCSize(size.width/scaleSd, size.height/scaleSd);
	node->setContentSize(newSize);
}

#pragma region Для premultiplied-pngшек

void SpriteHelper::SetBlendFuncForPremultiplied(CCSpriteBatchNode* node)
{
	//ccBlendFunc blendFunc = { GL_ONE, GL_ONE_MINUS_SRC_ALPHA }; //premultiplied
	//node->setBlendFunc(blendFunc);
}

void SpriteHelper::SetIsOpacityModifyForPremultiplied(CCSprite* sprite)
{
	//sprite->setIsOpacityModifyRGB(true);
}

CCTexture2D* SpriteHelper::LoadTextureAsPremultipliedPng(const char* fileimage)
{
#if COCOS2D_VERSION < 0x00020000
	CCImage::IsLoadAsPremultipliedPng() = true;	
	auto texture = CCTextureCache::sharedTextureCache()->addImage(GetTexturePath(fileimage).c_str());	
	CCImage::IsLoadAsPremultipliedPng() = false;
	return texture;
#else
	MY_ASSERT(false, "Not implemented");
	return NULL;
#endif
}

void SpriteHelper::DrawFilledPolygon(const CCPoint* poli, int numberOfPoints, GLenum drawingMode)
{
#if COCOS2D_VERSION < 0x00020000
    ccVertex2F* newPoint = new ccVertex2F[numberOfPoints];
    if (!newPoint)
    {
        return;
    }
    // Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
    // Needed states: GL_VERTEX_ARRAY, 
    // Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // iPhone and 32-bit machines
    if( sizeof(CCPoint) == sizeof(ccVertex2F) ) 
	{
		glVertexPointer(2, GL_FLOAT, 0, poli);
    } 
	else 
	{
        // 64-bit machines (Mac)
        for( int i=0; i<numberOfPoints;i++)
        {
            newPoint[i].x = poli[i].x;
            newPoint[i].y = poli[i].y;
        }

        glVertexPointer(2, GL_FLOAT, 0, newPoint );
    }

	glDrawArrays(drawingMode, 0, numberOfPoints);	

    // restore default state
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    delete[] newPoint;
#endif
}

string SpriteHelper::GetFontPath()
{
	return "fonts/arial16.fnt";
}

ccBlendFunc SpriteHelper::GetBlendFunc(GLenum src, GLenum dst)
{
	ccBlendFunc result = { src, dst };
	return result;
}

void SpriteHelper::SetSpriteRenderingWithoutAlpha(CCBlendProtocol* sprite)
{
	sprite->setBlendFunc(BLEND(GL_ONE, GL_ZERO));
}

CCMenuItemLabel* SpriteHelper::CreateLabel(CCObject* target, const char* text, const CCPoint& anchorPoint, 
	SEL_MenuHandler selector)
{
	auto label = CCLabelBMFont::create(text, GetFontPath().c_str());
	label->setAnchorPoint(ccp(0, 0));
	auto menuItem = CCMenuItemLabel::create(label, target, selector);
	menuItem->setAnchorPoint(anchorPoint);
	return menuItem;
}

//атлас texture разделить на totalFramesCount фреймов размером frameSize, с количеством строк rowCount 
//и количеством столбцов columnCount. И фреймы положить в массив result
void SpriteHelper::FillSpriteFramesFromAtlas(CCSpriteFrame** result, int totalFramesCount, CCTexture2D* texture, 
	CCSize frameSize, int rowCount, int columnCount)
{
	frameSize = CCSize(frameSize.width, frameSize.height);
	
	int framesCount = 0;
	for (int row = 0; row < rowCount; ++row)
		for (int column = 0; column < columnCount; ++column)
		{
			auto rect = Rect(column * frameSize.width, row * frameSize.height, frameSize.width, frameSize.height);
			result[framesCount] = new CCSpriteFrame();
			result[framesCount]->initWithTexture(texture, rect, false, CCPointZero, rect.size);

			++framesCount;
			if (framesCount >= totalFramesCount)
				break;
		}
}

#pragma region sprite creating

CCSpriteWrapper* SpriteHelper::CreateEmptySprite(CCNode* parent, int z) 
{
	auto sprite = new CCSpriteWrapper();
	if (parent != NULL)
	{
		parent->addChild(sprite, z);
		sprite->release();
	}
	else
	{
		sprite->autorelease();
	}
	return sprite;
}

CCSpriteWrapper* SpriteHelper::CreateSpriteWithTextureName(CCNode* parent, int z, const string& textureName,
	const CCPoint& anchorPoint/* = ccp(0.5f, 0.5f)*/) 
{
	auto texture = SpriteHelper::LoadTexture(textureName.c_str(), true);
	return CreateSpriteWithTexture(parent, z, texture, anchorPoint);
}

CCSpriteWrapper* SpriteHelper::CreateSpriteWithTexture(CCNode* parent, int z, CCTexture2D* texture,
	const CCPoint& anchorPoint/* = ccp(0.5f, 0.5f)*/) 
{
	auto sprite = SpriteHelper::CreateEmptySprite(parent, z);
	sprite->initWithTexture(texture);
	sprite->setAnchorPoint(anchorPoint);
	return sprite;
}

CCSpriteWrapper* SpriteHelper::CreateSpriteWithFrameName(CCNode* parent, int z, const string& frameName,
	const CCPoint& anchorPoint/* = ccp(0.5f, 0.5f)*/) 
{
	return CreateSpriteWithFrame(parent, z, SpriteHelper::LoadFrame(frameName), anchorPoint);
}

CCSpriteWrapper* SpriteHelper::CreateSpriteWithFrame(CCNode* parent, int z, CCSpriteFrame* frame,
	const CCPoint& anchorPoint/* = ccp(0.5f, 0.5f)*/) 
{
	auto sprite = new CCSpriteWrapper();
	sprite->autorelease();
	sprite->initWithSpriteFrame(frame);
	sprite->setAnchorPoint(anchorPoint);
	if (parent != nullptr)
		parent->addChild(sprite, z);
	return sprite;
}

void SpriteHelper::SetNodeSdScale(CCNode* node)
{
	if (!CommonProperties::IsLoadHDImages())
	{
		node->setScaleX(node->getScaleX() * 2);
		node->setScaleY(node->getScaleY() * 2);
	}
}


CCSpriteFrame* SpriteHelper::GetSpriteFrameFromTextureName(const string& textureName)
{
	auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(textureName.c_str());
	if (frame == NULL)
	{
		auto texture = SpriteHelper::LoadTexture(textureName);
		CCRect rect;
		rect.size = texture->getContentSize();

		frame = new CCSpriteFrame();
		frame->initWithTexture(texture, rect);
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, textureName.c_str());
		frame->release();
	}

	return frame;
}

CCSpriteFrame* SpriteHelper::GetSpriteFrameFromTexture(CCTexture2D* texture)
{
	CCRect rect;
	rect.size = texture->getContentSize();
	return CCSpriteFrame::createWithTexture(texture, rect);	
}

void SpriteHelper::SetBlendAdd(CCSprite* sprite) 
{ 
	sprite->setBlendFunc(BLEND(GL_ONE, GL_ONE)); 
}

void SpriteHelper::SetBlendMultiply(CCSprite* sprite) 
{ 
	sprite->setBlendFunc(BLEND(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA)); 
}

#pragma endregion

#pragma region Menu

CCMenuItemSprite* SpriteHelper::AddMenuItemToMenu(CCMenu* menu, 
	const string& frameNonActiveName, const string& frameActiveName, CCObject* target, SEL_MenuHandler selector)
{
	auto nonActive = new CCSprite();	
	nonActive->initWithSpriteFrame(SpriteHelper::LoadFrame(frameNonActiveName));
	auto active = new CCSprite();
	active->initWithSpriteFrame(SpriteHelper::LoadFrame(frameActiveName));
	//TODO: bluur while scaling CCMenuItemSprite!
	//scale with CCSprite is not correct! need to scale ok, because scale with CCMenuItemSprite not working

	auto menuItem = new CCMenuItemSprite();
	menuItem->INIT_FROM_NORMAL_SPRITE(nonActive, active, NULL, target, selector);

	menu->addChild(menuItem, 0);
	menuItem->release();
	active->release();
	nonActive->release();
	
	return menuItem;
}

CCMenu* SpriteHelper::CreateMenu(CCNode* parent, int z)
{
	//в CCMenu, если начал тач на кнопке1, а закончил этот тач на кнопке2, то активируется кнопка2.
	//это неправильное поведение. в этом случае, кнопка2 не должна активироваться. Должно быть так:
	//если начал тач на кнопке1 и закончил этот тач на кнопке1, то только тогда активируется кнопка1
	class CCFixedMenu : public CCMenu
	{
		protected_ virtual void ccTouchMoved(CCTouch* touch, CCEvent* event)
		{
			CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchMoved] -- invalid state");
			if (m_pSelectedItem == NULL)
				return;

			auto currentItem = this->itemForTouch(touch);
			if (currentItem != m_pSelectedItem) 
			{
				if (m_pSelectedItem->IS_SELECTED())
					m_pSelectedItem->unselected();
			}
			else
			{
				if (!m_pSelectedItem->IS_SELECTED())
					m_pSelectedItem->selected();
			}
		}

		protected_ virtual void ccTouchEnded(CCTouch* touch, CCEvent* event)
		{
			CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchEnded] -- invalid state");
			auto currentItem = this->itemForTouch(touch);
			if (m_pSelectedItem)
				m_pSelectedItem->unselected();

			if (m_pSelectedItem && currentItem == m_pSelectedItem)
				m_pSelectedItem->activate();

			m_eState = kCCMenuStateWaiting;
		}
	};

	//CCFixedMenu - like anonymus classes in Java )	
	//struct { int x, y; } a = { 0 };
	//a.x = 5;
	//struct { int x, y; } aaa[10] = { 0 }; //like anonymus classes in C# )
	//aaa[0].x = 3;
	//aaa[1] = (decltype(aaa[1]))a;  //хитро, хехе) это единственный способ хитрошмелить

	auto menu = new CCFixedMenu();
	parent->addChild(menu, z);
	menu->release();
	menu->init();
	menu->setPosition(CCPointZero);
	return menu;
}

#pragma endregion

#pragma endregion

//----------------CCNodeWrapperWrapper------------------
float CCNodeWrapperWrapper::FrameLeft()
{
	return Math::RoundF(X() - SpriteHelper::GetAnchorPointInPixels(_node).x); 
}

float CCNodeWrapperWrapper::FrameBottom() 
{
	return Math::RoundF(Y() - SpriteHelper::GetAnchorPointInPixels(_node).y); 
}

void CCNodeWrapperWrapper::SetAnchorPointInPixels(const CCPoint& anchorPointInPoints)
{
	SpriteHelper::SetAnchorPointInPixels(_node, anchorPointInPoints);
}

CCPoint CCNodeWrapperWrapper::ConvertToNodeSpace(const CCPoint& worldPoint)
{
	return InputHelper::ConvertToNodeSpace(_node, worldPoint);
}

CCPoint CCNodeWrapperWrapper::ConvertTouchToNodeSpace(CCTouch* touch)
{
	return InputHelper::ConvertTouchToNodeSpace(_node, touch);
}

CCPoint CCNodeWrapperWrapper::ConvertToWorldSpace(const CCPoint& nodePoint)
{
	return InputHelper::ConvertToWorldSpace(_node, nodePoint);
}


//-----------------CCSpriteWrapperWrapper----------------
float CCSpriteWrapperWrapper::Width() { return SpriteHelper::GetSpriteRect(_sprite).size.width; }
float CCSpriteWrapperWrapper::Height() { return SpriteHelper::GetSpriteRect(_sprite).size.height; }

float CCSpriteWrapperWrapper::FrameRight() { return FrameLeft() + Width(); }
float CCSpriteWrapperWrapper::FrameTop() { return FrameBottom() + Height(); }
CCPoint CCSpriteWrapperWrapper::PointCenter() { return ccp(FrameLeft() + Width()/2, FrameBottom() + Height()/2); }

Rect CCSpriteWrapperWrapper::GetRectUnscaled()
{
	auto pos = _sprite->getPosition();
	auto contentSize = SpriteHelper::GetContentSize(_sprite);
	auto anchorPoint = _sprite->getAnchorPoint();
	
	return Rect(pos.x - contentSize.width * anchorPoint.x, 
                        pos.y - contentSize.height * anchorPoint.y,
                        contentSize.width, contentSize.height);
}

void CCSpriteWrapperWrapper::SetScaleByScaleX(float scaleXAbsolute)
{
	float realScale = scaleXAbsolute/Width();
	_node->setScale(realScale);
}

void CCSpriteWrapperWrapper::SetScaleByScaleAbsolute(const CCSize& scaleSizeAbsolute)
{
	auto contentSize = SpriteHelper::GetContentSize(_sprite);
	_sprite->setScaleX(scaleSizeAbsolute.width/contentSize.width);
	_sprite->setScaleY(scaleSizeAbsolute.height/contentSize.height);
}