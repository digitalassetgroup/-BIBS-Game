#include "GAFAsset.h"
#include "GAFAnimatedObject.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"
#include "GAFSpriteWithAlpha.h"
#include "GAFStencilMaskSprite.h"
#include "GAFBlurredSprite.h"
#include "GAFFilterData.h"

#include "CCDirector.h"
#include "cocoa/CCDictionary.h"
#include "textures/CCTexture2D.h"
#include "sprite_nodes/CCSpriteFrame.h"
#include <limits>

#include "support/CCPointExtension.h"


static const char * kBlurFilterName = "Fblur";
static float const kAnimationFPS = 30.0;   // we keep this number 'almost' synchronized with web. The reason it's
// not 31 fps is that we will have some animation artifacts when running
// on 30/60 fps device.

static CCAffineTransform CGAffineTransformCocosFormatFromFlashFormat(CCAffineTransform aTransform)
{
    CCAffineTransform transform = aTransform;
    transform.b = - transform.b;
    transform.c = - transform.c;
    transform.ty = - transform.ty;
    return transform;
}

GAFAnimatedObject::GAFAnimatedObject()
:
_asset(NULL),
_subObjects(NULL),
_masks(NULL),
_extraFramesCounter(0),
_framePlayedDelegate(NULL)
{
}

GAFAnimatedObject::~GAFAnimatedObject()
{
	CC_SAFE_RELEASE(_asset);
	CC_SAFE_RELEASE(_subObjects);
	CC_SAFE_RELEASE(_masks);
}

GAFAnimatedObject * GAFAnimatedObject::create(GAFAsset * anAsset)
{
	GAFAnimatedObject * ret = new GAFAnimatedObject();
	if (ret && ret->init(anAsset))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_RELEASE(ret);
	return NULL;
}


GAFAnimatedObject * GAFAnimatedObject::create(const char * jsonPath)
{
	GAFAsset * asset = GAFAsset::create(jsonPath);
	return asset->createObject();
}


GAFAnimatedObject * GAFAnimatedObject::createAndRun(const char * jsonPath, bool looped)
{
	GAFAsset * asset = GAFAsset::create(jsonPath);
	return asset->createObjectAndRun(looped);
}

bool GAFAnimatedObject::init(GAFAsset * anAsset)
{
	if (!anAsset)
	{
		return false;
	}
	if (!GAFAnimation::init(anAsset))
	{
		return false;
	}
	if (_asset != anAsset)
	{
		CC_SAFE_RELEASE(_asset);
		_asset = anAsset;
		CC_SAFE_RETAIN(_asset);
	}	
	CC_SAFE_RELEASE(_subObjects);
	_subObjects = CCDictionary::create();
	CC_SAFE_RETAIN(_subObjects);
	
	CC_SAFE_RELEASE(_masks);
	_masks = CCDictionary::create();
	CC_SAFE_RETAIN(_masks);
	
	_FPSType = PCAnimationFPSType_30;
	_extraFramesCounter = 0;
	_animationsSelectorScheduled = false;	
	return true;
}

static bool util_ccarray_contains_string(CCArray * array, const std::string& str)
{
	if (!array)
	{
		return false;
	}
	for (int i = 0; i < array->count(); ++i)
	{
		CCString * obj = (CCString *)array->objectAtIndex(i);
		if (obj->getCString() == str)
		{
			return true;
		}
	}
	
	return false;
}

CCDictionary * GAFAnimatedObject::masksAssociationStructureFromAnimationObjectIds
(CCArray * anAnimationObjectIds, CCArray * anAnimationFrames, CCArray **aMaskedObjectIds)
{
    CCDictionary * maskedObjectsStructure = new CCDictionary();
    
    if (aMaskedObjectIds)
    {
        *aMaskedObjectIds = CCArray::create();
    }
    
    CCArray *animationObjectIds = CCArray::createWithArray(anAnimationObjectIds);
    for (int i = 0; i < anAnimationFrames->count(); ++i)
    {
		GAFAnimationFrame * frame = (GAFAnimationFrame*)anAnimationFrames->objectAtIndex(i);
        for (int j = 0; j < frame->objectStates()->count(); ++j)
        {
			GAFSubobjectState *state = (GAFSubobjectState *)frame->objectStates()->objectAtIndex(j);
            if (state->maskObjectId.length())
            {
                CCArray *maskSubObjects = (CCArray*)maskedObjectsStructure->objectForKey(state->maskObjectId);
                if (!maskSubObjects)
                {
                    maskSubObjects = CCArray::create();
					maskedObjectsStructure->setObject(maskSubObjects, state->maskObjectId);
                }
				
				CCString * str = CCString::create(state->objectId);
				if (!util_ccarray_contains_string(maskSubObjects, str->getCString()))
				{
					maskSubObjects->addObject(str);
				}
				if (aMaskedObjectIds)
				{
					if (!util_ccarray_contains_string(*aMaskedObjectIds, str->getCString()))
					{
						(*aMaskedObjectIds)->addObject(str);
					}
				}
				// this can be improved in original code
				for (int k = 0; k < animationObjectIds->count(); ++k)
				{
					CCString * obj = (CCString*)(animationObjectIds->objectAtIndex(k));
					if (obj && state->objectId == obj->getCString())
					{
						animationObjectIds->fastRemoveObjectAtIndex(k);
						--k;
					}
				}
            }
        }
        
        if (!animationObjectIds->count())
		{
			break;
		}
    }
	return maskedObjectsStructure;
}

GAFSprite * GAFAnimatedObject::getNamedSubobject(const char * name)
{
	if (!name)
	{
		return 0;
	}
	CCDictionary * namedParts = _asset->namedParts();
	if (!namedParts)
	{
		return 0;
	}
	const char * rawName = 0;
	CCDictElement* pElement = 0;
    CCDICT_FOREACH(namedParts, pElement)
    {
		CCString * str = (CCString*)pElement->getObject();
		if (0 == strcmp(str->getCString(), name))
		{
			rawName = pElement->getStrKey();
			break;
		}
    }
	if (!rawName)
	{
		return 0;
	}
	return getSubobjectByRawName(rawName);
}

GAFSprite * GAFAnimatedObject::getSubobjectByRawName(const char * name)
{
	if (!_subObjects)
	{
		return 0;
	}
	return (GAFSprite *)_subObjects->objectForKey(name);
}

void GAFAnimatedObject::addSubObjectsUsingAnimationObjectsDictionary(CCDictionary * anAnimationObjects,
CCDictionary * anAnimationMasks, CCArray * anAnimationFrames)
{
    CCArray * maskedObjectIds = NULL;
	CCArray * allKeys = anAnimationObjects->allKeys();
    CCDictionary *maskedObjectsStructure = masksAssociationStructureFromAnimationObjectIds(allKeys, anAnimationFrames, &maskedObjectIds);
    
	
	CCDictElement* pElement = NULL;
    CCDICT_FOREACH(anAnimationObjects, pElement)
    {
		
        CCString * atlasElementId = (CCString *)anAnimationObjects->objectForKey(pElement->getStrKey());
        CCSpriteFrame * spriteFrame = NULL;
		GAFTextureAtlas * atlas = _asset->textureAtlas();
		
        GAFTextureAtlasElement *element = (GAFTextureAtlasElement *)atlas->elements()->objectForKey(atlasElementId->getCString());
		
		if (element)
		{
			if (atlas->textures()->count() >= element->atlasIdx + 1)
			{
				CCTexture2D * texture = (CCTexture2D *)atlas->textures()->objectAtIndex(element->atlasIdx);
				spriteFrame = CCSpriteFrame::createWithTexture(texture, element->bounds);
			}
			else
			{
				CCLOGERROR("Cannot add sub object with Id: %s, atlas with idx: %d not found.", atlasElementId->getCString(), element->atlasIdx);
			}
		}
		        
		      
        if (spriteFrame)
        {
            GAFSpriteWithAlpha *sprite = new GAFSpriteWithAlpha();
			sprite->initWithSpriteFrame(spriteFrame);
			
            sprite->objectId = atlasElementId->getCString();
            
            sprite->setVisible(false);
			CCPoint pt = CCPointMake(0 - (0 - (element->pivotPoint.x / sprite->getContentSize().width)),
									 0 + (1 - (element->pivotPoint.y / sprite->getContentSize().height)));
            sprite->setAnchorPoint(pt);
			sprite->setUseExternalTransform(true);
            
            // Add to hierarchy
			_subObjects->setObject(sprite, pElement->getStrKey());
            // Check if the object is masked (don't add it to visual hierarchy then)
            if (!util_ccarray_contains_string(maskedObjectIds, pElement->getStrKey()))
            {
				addChild(sprite);
            }
			sprite->release();
        }
        else
        {
            CCLOGERROR("Cannot add subnode with AtlasElementName: %s, not found in atlas(es). Ignoring.", atlasElementId->getCString());
        }
    }
    

	CCDICT_FOREACH(maskedObjectsStructure, pElement)
	{
		CCString * atlasElementId = (CCString *)anAnimationMasks->objectForKey(pElement->getStrKey());
		
		CCSpriteFrame *spriteFrame = NULL;
		GAFTextureAtlas * atlas = _asset->textureAtlas();
		GAFTextureAtlasElement *element = (GAFTextureAtlasElement *)atlas->elements()->objectForKey(atlasElementId->getCString());
		
		if (element)
		{
			spriteFrame = CCSpriteFrame::createWithTexture(atlas->texture(), element->bounds);
			
			if (spriteFrame)
			{
				GAFStencilMaskSprite *mask = new GAFStencilMaskSprite();
				mask->initWithSpriteFrame(spriteFrame);
				
				CCPoint pt = CCPointMake(0 - (0 - (element->pivotPoint.x / mask->getContentSize().width)),
										 0 + (1 - (element->pivotPoint.y / mask->getContentSize().height)));
				
				mask->setAnchorPoint(pt);
				mask->setUseExternalTransform(true);
				
				_masks->setObject(mask, pElement->getStrKey());
				addChild(mask);
				
				// Adding masked objects to mask
				CCArray * arr = (CCArray*) maskedObjectsStructure->objectForKey(pElement->getStrKey());
				if (arr)
				{
					for (int j = 0; j < arr->count(); ++j)
					{
						CCString * objectId = (CCString*)arr->objectAtIndex(j);
						GAFSprite * subobject = (GAFSprite*)_subObjects->objectForKey(objectId->getCString());
						if (subobject)
						{
							mask->addMaskedObject(subobject);
						}
						else
						{
							CCLOGERROR("Object with id:%s is not in 'animationObjects' list in config json.", objectId->getCString());
						}
					}
				}
			}
		}
	}
	CC_SAFE_RELEASE(maskedObjectsStructure);
}

void GAFAnimatedObject::removeAllSubObjects()
{
	CCDictElement* pElement = NULL;
    CCDICT_FOREACH(_subObjects, pElement)
    {
        GAFSprite* sprite = (GAFSprite*)pElement->getObject();
		sprite->setVisible(false);
		sprite->removeFromParentAndCleanup(true);
    }
	_subObjects->removeAllObjects();
}

void GAFAnimatedObject::setSubobjectsVisible(bool visible)
{
	CCDictElement* pElement = NULL;
    CCDICT_FOREACH(_subObjects, pElement)
    {
        GAFSprite* sprite = (GAFSprite*)pElement->getObject();
		sprite->setVisible(visible);
    }
}

void GAFAnimatedObject::processAnimations(float dt)
{
	if (++_extraFramesCounter >= numberOfGlobalFramesForOneAnimationFrame())
    {
        _extraFramesCounter = 0;
		if (!isDone() && isAnimationRunning())
		{
			step();
		}
		if (_framePlayedDelegate)
		{
			_framePlayedDelegate->onFramePlayed(this, currentFrameIndex());
		}
    }
}


CCPoint GAFAnimatedObject::pupilCoordinatesWithXSemiaxis(float anXSemiaxis, float anYSemiaxis, CCPoint aCenter, CCPoint anExternalPoint)
{
    CCPoint rePoint = CCPointZero;
    float const firstConst = (anYSemiaxis * anYSemiaxis);
    float const secondConst = firstConst/(anXSemiaxis * anXSemiaxis);
    float length = std::numeric_limits<float>::max();
    float tmpLenght = length;
    float xStart = - anXSemiaxis;
    float xEnd = 0;
	
    if (((anExternalPoint.x - aCenter.x) * (anExternalPoint.x - aCenter.x)) / (anXSemiaxis * anXSemiaxis) +
        ((anExternalPoint.y - aCenter.y) * (anExternalPoint.y - aCenter.y)) / (anYSemiaxis * anYSemiaxis) < (1.0 + FLT_EPSILON))
    {
        return anExternalPoint;
    }
    if (anExternalPoint.x > aCenter.x)
    {
        xStart = 0;
        xEnd = anXSemiaxis;
    }
    for (float i = xStart; i <= xEnd; i += 0.5f)
    {
        float y = sqrtf(firstConst - secondConst * (i * i));
        if (anExternalPoint.y > aCenter.y)
        {
            y = - y;
        }
        float dX = anExternalPoint.x - (aCenter.x + i);
        float dY = anExternalPoint.y - (aCenter.y - y);
        tmpLenght = sqrtf(dX * dX + dY * dY);
        if (tmpLenght < length)
        {
            length = tmpLenght;
            rePoint = CCPointMake(aCenter.x + i, aCenter.y - y);
        }
    }
    return rePoint;
}

GAFSprite * GAFAnimatedObject::subObjectForInnerObjectId(CCString * anInnerObjectId)
{
	CCDictElement* pElement = NULL;
    CCDICT_FOREACH(_subObjects, pElement)
    {
        GAFSprite* anim = (GAFSprite*)pElement->getObject();
		if (anim->objectId == anInnerObjectId->getCString())
		{
			return anim;
		}
    }
	return NULL;
	
}

CCDictionary * GAFAnimatedObject::subObjects()
{
	return _subObjects;
}

CCDictionary * GAFAnimatedObject::masks()
{
	return _masks;
}

void GAFAnimatedObject::start()
{

	GAFAnimation::start();
	removeAllSubObjects();
    addSubObjectsUsingAnimationObjectsDictionary(_asset->objects(), _asset->masks(), _asset->animationFrames());
	
	schedule(SEL_SCHEDULE(&GAFAnimatedObject::processAnimations));
	_animationsSelectorScheduled = true;
	

}

void GAFAnimatedObject::stop()
{
	GAFAnimation::stop();
	
	unschedule(SEL_SCHEDULE(&GAFAnimatedObject::processAnimations));
	_animationsSelectorScheduled = false;
}

int GAFAnimatedObject::numberOfGlobalFramesForOneAnimationFrame()
{
	float globalFPS = roundf(1.0 / CCDirector::sharedDirector()->getAnimationInterval());
    
    if (globalFPS > (float)_FPSType - FLT_EPSILON)
    {
        return (int)roundf(globalFPS / (float)_FPSType);
    }
    else
    {
        return 1;
    }
}

void GAFAnimatedObject::processAnimation()
{
    GAFAnimationFrame *currentFrame = (GAFAnimationFrame*)_asset->animationFrames()->objectAtIndex(_currentFrameIndex);
	setSubobjectsVisible(false);
	
	CCArray * arr = currentFrame->objectStates();
	
	if (arr)
	{
		int n = arr->count();
		for (int i = 0; i < n; ++i)
		{
			
			GAFSubobjectState *state = (GAFSubobjectState*)arr->objectAtIndex(i);
			//if (state.petPartName == nil || [state.petPartName isEmpty])
			{
				CCDictionary * subobjects = subObjects();
				GAFSpriteWithAlpha *subObject =  NULL;
				if (subobjects)
				{
					subObject = (GAFSpriteWithAlpha *)subobjects->objectForKey(state->objectId);
				}
				
				if (subObject)
				{
					subObject->setVisible(true);
					
					// Validate sprite type (w/ or w/o filter)
					GAFBlurFilterData *blurFilter = NULL;
					
					if (state->filters())
					{
						blurFilter = (GAFBlurFilterData *) state->filters()->objectForKey(kBlurFilterName);
					}
					
					
					if (!blurFilter && dynamic_cast<GAFBlurredSprite*>(subObject))
					{
						CCLOGINFO("Converted filtered sprite with ID: %s to simple.", state->objectId.c_str());
						// revert sprite back to simple one
						GAFBlurredSprite *filteredSprite = (GAFBlurredSprite *)subObject;
						// CHECK HERE
						removeChild(filteredSprite, false);
						subObject = filteredSprite->sourceSprite();
						addChild(subObject);
						subObjects()->setObject(filteredSprite->sourceSprite(), state->objectId);
					}
					else
						if (blurFilter)
						{
							if (!dynamic_cast<GAFBlurredSprite*>(subObject))
							{
								CCLOGINFO("Converted simple sprite with ID: %s to filtered.", state->objectId.c_str());
								// convert sprite to filtered one
								GAFBlurredSprite *filteredSprite = new GAFBlurredSprite();
								filteredSprite->initWithGAFSprite(subObject, blurFilter->blurSize());
								filteredSprite->updateDynamicTexture();
								
								subObjects()->setObject(filteredSprite, state->objectId);
								filteredSprite->release();
								
								removeChild(subObject, false);
								subObject = filteredSprite;
								addChild(filteredSprite);
							}
							else
							{
								// Check if filter parameters match
								GAFBlurredSprite *filteredSprite = (GAFBlurredSprite *)subObject;
								CCSize roundedBlurSize = CCSizeMake(ceilf(blurFilter->blurSize().width),
																	ceilf(blurFilter->blurSize().height));
								
								CCSize normalizedBlurSize = filteredSprite->normalizedBlurSizeFromNeededBlurSize(roundedBlurSize);
								
								if (filteredSprite->blurSize().width != normalizedBlurSize.width || filteredSprite->blurSize().height != normalizedBlurSize.height)
								{
									CCLOGINFO("Modifying filtered sprite ID: %s parameters.", state->objectId.c_str());
									
									filteredSprite->setBlurSize(roundedBlurSize);
									filteredSprite->updateDynamicTexture();
								}
							}
						}
					
					// Update object position and alpha
					subObject->setExternaTransform( CGAffineTransformCocosFormatFromFlashFormat(state->affineTransform));
					
					if (subObject->getZOrder() != state->zIndex)
					{
						subObject->setZOrder(state->zIndex);
						GAFStencilMaskSprite::updateMaskContainerOf(subObject);
					}
					
					// Set alpha
					subObject->setVisible(state->isVisisble());
					subObject->setColorTransform(state->colorMults(), state->colorOffsets());
				}
				else
				{
					CCDictionary * aMasks = masks();
					
					GAFSprite * mask = NULL;
					if (aMasks)
					{
						mask = (GAFSprite *)aMasks->objectForKey(state->objectId);
					}
					if (mask)
					{
						mask->setExternaTransform(CGAffineTransformCocosFormatFromFlashFormat(state->affineTransform));
						
						if (mask->getZOrder() != state->zIndex)
						{
							mask->setZOrder(state->zIndex);
							GAFStencilMaskSprite::updateMaskContainerOf(subObject);
						}
					}
				}
			}
		}
	}
}


