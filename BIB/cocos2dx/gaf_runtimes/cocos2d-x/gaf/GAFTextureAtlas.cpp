#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFData.h"
#include "platform/CCImage.h"
#include "textures/CCTexture2D.h"
#include "cocoa/CCArray.h"
#include "cocoa/CCDictionary.h"
#include "platform/CCFileUtils.h"

#if CC_ENABLE_CACHE_TEXTURE_DATA
#include "textures/CCTextureCache.h"
#endif

static const char * kElementsKey     = "elements";
static const char * kImagePathKey    = "source";

GAFTextureAtlas::GAFTextureAtlas()
:
_loaded(false),
_images(NULL),
_textures(NULL),
_elements(NULL)
{
}

GAFTextureAtlas::~GAFTextureAtlas()
{
	CC_SAFE_RELEASE(_images);
	CC_SAFE_RELEASE(_textures);
	CC_SAFE_RELEASE(_elements);
}

GAFTextureAtlas * GAFTextureAtlas::create(const char * jsonPath, CCDictionary * aConfigDictionary, CCArray * anImageDatas)
{
	GAFTextureAtlas *pRet = new GAFTextureAtlas();
    if (pRet && pRet->init(jsonPath, aConfigDictionary, anImageDatas))
    {
		pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}


bool GAFTextureAtlas::init(const char * jsonPath, CCDictionary * aConfigDictionary, CCArray * anImageDatas)
{
	if (!aConfigDictionary || !anImageDatas)
	{
		CCLOG("parameters should not equal to nil");
		return false;
	}
	
	CC_SAFE_RELEASE(_images);
	_images = new CCArray();
	
	for (int i = 0; i < anImageDatas->count(); ++i)
	{
		CCDictionary * data = (CCDictionary*)anImageDatas->objectAtIndex(i);
		CCString * source = (CCString *)data->objectForKey(kImagePathKey);
		CCImage * image = new CCImage();
		const char * path = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(source->getCString(), jsonPath);
		image->initWithImageFile(path);
		_images->addObject(image);
		image->release();
	}
	loadElementsFromAnimationConfigDictionary(aConfigDictionary);
	return true;
}

bool GAFTextureAtlas::loadElementsFromAnimationConfigDictionary(CCDictionary * aConfigDictionary)
{
    CCArray   * nElements     = (CCArray*)   aConfigDictionary->objectForKey(kElementsKey);
	
	if (nElements)
	{
		int s = nElements->count();
		
		CC_SAFE_RELEASE(_elements);
		_elements = CCDictionary::create();
		CC_SAFE_RETAIN(_elements);
		
		for (int i = 0; i < s; ++i)
		{
			CCObject * nElement = nElements->objectAtIndex(i);
			CCDictionary * dict = dynamic_cast<CCDictionary*>(nElement);
			if (dict)
			{
				GAFTextureAtlasElement * element = GAFTextureAtlasElement::create(dict);
				if (element)
				{
					_elements->setObject(element, element->name);
				}
			}
			else
			{
				CCLOGERROR("Error when parsing texture atlas element JSON. Atlas element not of CCDictionary type");
			}
		}
	}
	else
	{
		CCLOGERROR("Error when parsing texture atlas element JSON.");
		return false;
	}
	_loaded = true;
	return true;
}


CCDictionary * GAFTextureAtlas::elements()
{
	return _elements;
}

CCImage     * GAFTextureAtlas::image()
{
	if (_images && _images->count() > 0)
	{
		return (CCImage*) _images->objectAtIndex(0);
	}
	return NULL;
}

CCArray     * GAFTextureAtlas::images()
{
	return _images;
}

CCTexture2D * GAFTextureAtlas::texture()
{
	if (_textures && _textures->count() > 0)
	{
		return (CCTexture2D*) _textures->objectAtIndex(0);
	}
	return NULL;
}

CCArray * GAFTextureAtlas::textures()
{
	if (!_textures)
	{
		_textures = CCArray::createWithCapacity(_images->count());
		for (int i = 0; i < _images->count(); ++i)
		{
			CCTexture2D * texture = new CCTexture2D();
			CCImage * image = (CCImage*)_images->objectAtIndex(i);
			texture->initWithImage(image);
			_textures->addObject(texture);
#if CC_ENABLE_CACHE_TEXTURE_DATA
			VolatileTexture::addCCImage(texture, image);
#endif
			texture->release();
		}
		_textures->retain();
	}
	return _textures;
}