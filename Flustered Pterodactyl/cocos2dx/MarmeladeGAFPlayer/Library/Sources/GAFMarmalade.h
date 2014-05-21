#pragma once

#ifndef __GAF_MARMALDE_H___
#define __GAF_MARMALDE_H___

#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFShaderProgram.h"
#include "cocoagaf/GAFAffineTransform.h"

namespace GAF
{
	
/// @class GAFSpriteCallback
/// @note this class is used to quiry sprte parameters that have non-primitive data types. Now they are only shader parameters.
class GAFSpriteCallback
{
public:
	virtual void getParameter(int parameter, void * result) = 0;
};


	typedef void (releaseFn)(void *);
	typedef void (createFn)(void *);
	typedef void (GAF::CCObject::*GAFSEL_CallFunc)(float);
/// @class GAFMarmaladeGFX
/// @note You must call onFrame() every frame after your rendering step is over or at the beginning of the frame.
///
/// To implemnet rendering playback other than Cocos2dx you must implement all functions
/// listed in GAFMarmaladeGFX. For sample implementation you can check current Cocos2dx-bases implementation.
/// Our advice is to contact gafmedia.com before you start to work on your own backend - most likely
/// backend you need is already in development.
/// We tryed to be as less demanding as we can in terms of backend features you need to implement GAFMarmaladeGFX.
/// If your game engine features sprites, transformation and OpenGL ES shaders it should not take too long to implement
/// playback. If you have only OpenGL ES immediate mode then things are more diffictult to you - you have to implement
/// mentioned abstructions. The most important thing with immediate mode implementation is carefully set and restore
/// OpenGL states. 
	class GAFMarmaladeGFX
	{
	public:
/// You backend must have "image" abstraction.
/// @param path - resource path. This can be anything, but most likely it will be file system path. If you are going to
/// do something else note that your code should respect image paths produced by GAF converter.
		static void * createImage(const char * path);
/// @param image - created by GAFMarmaladeGFX::createImage
		static void   releaseImage(void * image);
/// You backend must have "texture" abstraction.
/// @param image - image created by GAFMarmaladeGFX::createImage
		static void * createTexture(void * image);
/// @param texture - created by GAFMarmaladeGFX::createTexture
		static void   releaseTexture(void * texture);
/// You backend must have "spriteContainer" abstraction. Most likely it is sprite itself.
		static void * createSpriteContainer();
/// @param spriteContaner - create by GAFMarmaladeGFX::createSpriteContainer
		static void   releaseSpriteContainer(void * spriteContaner);
/// @note sprites located in sprite continer must respect parent's scale
		static void   setSpriteContainerScale(void * spriteContaner, float scale);
/// @note sprites located in sprite continer must respect parent's position		
		static void   setSpriteContainerPosition(void * sprteContainer, float x, float y);
/// @param sprte - created by GAFMarmaladeGFX::createSprite
		static void   setSpritePosition(void * sprte, float x, float y);
/// @param sprte - created by GAFMarmaladeGFX::createSprite
static void   setSpriteLocator(void * sprte, bool locator);
/// You backend must have "sprite" abstraction.
		static void *        createSprite(GAF::GAFSpriteCallback * cb);
/// You backend must have "stencil sprite" abstraction. It must support masked objects.
		static void * createStencilSprite(GAF::GAFSpriteCallback * cb);
/// Adds masked object to stencil sprite
/// @param stencilSprite - created by GAFMarmaladeGFX::stencilSprite
/// @param maskedObject  - cretaed by GAFMarmaladeGFX::createSprite
		static void   stencilSpriteAddMaskedObject(void * stencilSprite, void * maskedObject);
/// Used to maintain correct Z order of masked object. Your implementation should ensure that masked objects order is valid.
/// @param stencilSprite - created by GAFMarmaladeGFX::stencilSprite
		static void   stencilSpriteUpdateMaskContainerOf(void * stencilSprite);
/// Sprite initialization
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @param texture - created by GAFMarmaladeGFX::createTexture
/// @param x, y, w, h - sprite rect
/// @param rotated - if sprite rect rotated or not
		static bool   initSprite(void * sprite, void * texture, float x, float y, float w, float h, bool rotated);
/// @param sprite - created by createSprite
		static void   releaseSprite(void * sprite);
/// Your backend must support affine transformations for sprites
/// @param sprite - creatd by GAFMarmaladeGFX::createSprite
/// @param transform - transformation
		static void   setSpriteTransform(void * sprite, const CCAffineTransform& transform);
/// Your backend must have sprite frame abstraction. Basially it is just texture and texture rect.
/// @param texture - created by GAFMarmaladeGFX::createTexture
/// @param x, y, w, h - texture rect
		static void * createSpriteFrame(void * texture, float x, float y, float w, float h);
/// @param spriteFrame - created by GAFMarmaladeGFX::createSpriteFrame
		static void   releaseSpriteFrame(void * spriteFrame);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @return content size width
		static float  getSpriteContentSizeWidth(void * sprite);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @return content size height
		static float  getSpriteContentSizeHeight(void * sprite);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @return x position of sprite's anchor point
		static float  getSpriteAnchorPositionX(void * sprite);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @return y position of sprite's anchor point
		static float  getSpriteAnchorPositionY(void * sprite);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @return z order of the sprite
		static int    getSpriteZorder(void * sprite);
/// set GAF atlas scale of the sprite
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void   setSpriteAtlasScale(void * sprite, float scale);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void * getSpriteParent(void * sprite);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void   setSpriteBlurRadius(void * sprite, float width, float height);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void   setSpriteShader(void * sprite, void * shader);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @param visible - sprite visibility 
		static void   setSpriteVisible(void * sprite, bool visible);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @param x,y - ahchor point
		static void   setSpriteAnchorPoint(void * sprite, float x, float y);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @param zOrder - z order
		static void   setSpriteZorder(void * sprite, int zOrder);
		
/// Must set parameters that are equivalent to the following : src - ONE dst - ONE_MINUS_SRC_ALPHA
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void   setAlphaBlendingForSprite(void * sprite);
/// @param spriteContainer - created by GAFMarmaladeGFX::createSpriteContainer
/// @param zOrder - z order
		static void   setSpriteContainerZorder(void * spriteContainer, int zOrder);
/// adds sprite container to scene
/// @param scene - object that can contin sprite containers.
/// @param spriteContainer - created by GAFMarmaladeGFX::createSpriteContainer
		static void   sceneAddSpriteContainer(void * scene, void * spriteContainer);
/// removes sprite container from scene
/// @param scene - object that can contin sprite containers.
/// @param spriteContainer - created by GAFMarmaladeGFX::createSpriteContainer
		static void   sceneRemoveSpriteContainer(void * scene, void * spriteContainer);
/// adds sprite to sprite container
/// @param spriteContainer - created by GAFMarmaladeGFX::createSpriteContainer
/// @param sprite - created by GAFMarmaladeGFX::createSprite
		static void   spriteContainerAddSprite(void * spriteContainer, void * sprite);
/// removes sprite to sprite container
/// @param spriteContainer - created by GAFMarmaladeGFX::createSpriteContainer
/// @param sprite - created by createSprite
/// @param cleanup - cleanup sprite resources or not
		static void   spriteContainerRemoveSprite(void * spriteContainer, void * sprite, bool cleanup);
/// @param sprite - created by GAFMarmaladeGFX::createSprite
/// @param spriteFrame - created by GAFMarmaladeGFX::createSpriteFrame
		static bool initSpriteWithSpriteFrame(void * sprite, void * spriteFrame);
/// used to update animations internally
		static void  scheduleUpdatePerFrame(CCObject * object, GAFSEL_CallFunc);
		static void  unscheduleUpdatePerFrame(CCObject * object, GAFSEL_CallFunc);
/// used to get interval between frames in seconds
		static float getAnimationInterval();
/// @note You must call onFrame() every frame after your rendering step is over or at the beginning of the frame.
		static void  onFrame();
	};
}



#endif // __GAF_MARMALDE_H___
