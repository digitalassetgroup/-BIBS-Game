#pragma once

#ifndef __GAF_TEXTURE_ATLAS___
#define __GAF_TEXTURE_ATLAS___

#include "cocoagaf/GAFObject.h"

namespace GAF
{
	class CCImage;
	class CCArray;
	class CCTexture2D;
	class CCDictionary;

class GAFTextureAtlas : public CCObject
{
public:
	~GAFTextureAtlas();
	
	static GAFTextureAtlas * create(const char * aTexturesDirectory, CCDictionary * aTextureAtlasConfigDictionary);
	bool init(const char * aTexturesDirectory, CCDictionary * aTextureAtlasConfigDictionary);
	
	inline bool loaded() const
	{
		return _loaded;
	}
	
	CCImage     * image();
	CCArray     * images();
	
	CCTexture2D * texture();
	CCArray     * textures();
	
	CCDictionary * elements();
	
	bool loadElementsFromAnimationConfigDictionary(CCDictionary * aConfigDictionary);
protected:
	GAFTextureAtlas();
private:
	bool           _loaded;
	CCArray      *  _images;
	CCArray      *  _textures;
	CCDictionary * _elements;
};
	
} // namespace GAF

#endif // __GAF_TEXTURE_ATLAS___
