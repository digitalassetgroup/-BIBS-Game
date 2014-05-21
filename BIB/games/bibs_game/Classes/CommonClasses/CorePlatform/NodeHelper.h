#pragma once
#ifndef NodeHelper_h
#define NodeHelper_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include <base_nodes/CCNode.h>
using namespace cocos2d;

#define CCArray_ForEach(Type, Element, Collection) \
	{ \
		CCArray* children = Collection; \
		CCObject* pObject = NULL; \
		CCARRAY_FOREACH(children, pObject) \
		{ \
			if(pObject == NULL) \
				break; \
			\
			Type Element = (Type)pObject; 

#define CCArray_ForEachEnd() \
		} \
	}


namespace NodeHelper
{
	void PauseAllNodes(CCNode* parentNode);
	void ResumeAllNodes(CCNode* parentNode);
	void PauseAllNodesRecoursive(CCNode* node);
	void ResumeAllNodesRecoursive(CCNode* node);

	void CopyPropertiesFromNode(CCNode* source, CCNode* destination);
	void AddToOtherNode(CCNode* parent, CCNode* node, int z = -100);	
	void UnionNodesToOneNode(CCNode* resultNode, const vector<CCNode*>& nodesToUnion);
	void ChangeNodeAnchorPoint(CCNode* sprite, const CCPoint& anchorPointNew);
	void ChangeZ(CCNode* node, int z);
	bool IsNodeContainsPoint(CCNode* sprite, const CCPoint& point);
	bool IsNodeContainsTouchPoint(CCNode* node, CCSet* touches);	
};

#endif
