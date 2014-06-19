#include "GAFNode.h"

GAFNode::GAFNode()
:
_useExternalTransform(false)
{
	
}

GAFNode * GAFNode::create()
{
	GAFNode * ret = new GAFNode();
	ret->autorelease();
	return ret;
}

void GAFNode::setExternaTransform(const CCAffineTransform& transform)
{
	if (!CCAffineTransformEqualToTransform(_externalTransform, transform))
	{
		_externalTransform = transform;
		m_bTransformDirty = true;
		m_bInverseDirty = true;
	}
}

CCAffineTransform GAFNode::nodeToParentTransform(void)
{
	if (_useExternalTransform)
	{
		if (m_bTransformDirty)
		{
			m_sTransform = CCAffineTransformTranslate(_externalTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
			m_bTransformDirty = false;
			m_bInverseDirty   = true;
		}
		return m_sTransform;
		
	}
	else
	{
		return CCNode::nodeToParentTransform();
	}
}