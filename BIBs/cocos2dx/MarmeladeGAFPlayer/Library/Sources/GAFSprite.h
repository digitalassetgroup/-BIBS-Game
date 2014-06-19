#pragma once

#ifndef __GAF_SPRITE__
#define __GAF_SPRITE__

#include "cocoagaf/GAFObject.h"
#include "cocoagaf/GAFAffineTransform.h"
#include "cocoagaf/GAFGeometry.h"
#include "GAFMarmalade.h"
#include <string>

namespace GAF
{

/// @class GAFSprite 
///	This is utility class used by GAF playback. It does not perform rendering or use OpenGL.
/// Instead, it references backend object via GAFSprite::getExternalPointer.
class GAFSprite : public CCObject, public GAFSpriteCallback
{
public:
	GAFSprite();
	~GAFSprite();
/// object identifier found in JSON
	std::string objectId;
	void setExternaTransform(const CCAffineTransform& transform);
	bool initWithTexture(void *pTexture, const CCRect& rect, bool rotated);
/// @return external pointer to GAF rendering backend object	
	void * getExternalPointer()
	{
		return _externalPointer;
	}
/// @param pointer - external pointer to GAF rendering backend object
	void setExternalPointer(void * pointer)
	{
		_externalPointer = pointer;
	}
/// set visibilty of the sprite
	void setVisible(bool visible);
	virtual void initExternal();
	void getParameter(int parameter, void * result);
private:
	void * _externalPointer;
};

} // namespace GAF

#endif // __GAF_SPRITE__
