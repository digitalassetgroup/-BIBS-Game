#pragma once

#ifndef __GAF_NODE__
#define __GAF_NODE__

#include "base_nodes/CCNode.h"
#include "cocoa/CCAffineTransform.h"

using namespace cocos2d;

class GAFNode : public CCNode
{
public:
	GAFNode();
	static GAFNode * create();
	void setExternaTransform(const CCAffineTransform& transform);
    virtual CCAffineTransform nodeToParentTransform(void);
private:
	bool _useExternalTransform;
	CCAffineTransform _externalTransform;
};


#endif // __GAF_NODE__
