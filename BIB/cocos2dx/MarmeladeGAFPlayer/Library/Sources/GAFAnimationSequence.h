#pragma once

#ifndef __GAF_ANIMATION_SEQUENCE_H__
#define __GAF_ANIMATION_SEQUENCE_H__

#include "cocoagaf/GAFObject.h"
#include <string>

namespace GAF
{

class GAFAnimationSequence : public CCObject
{
public:
	std::string name;
	int startFrameNo;
	int endFrameNo;
	inline int length() const
	{
		return endFrameNo - startFrameNo;
	}
};
	
}// namespace GAF

#endif // __GAF_ANIMATION_SEQUENCE_H__
