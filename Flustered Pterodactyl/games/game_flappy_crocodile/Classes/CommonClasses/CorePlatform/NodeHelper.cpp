#include "NodeHelper.h"
#include "GeometryHelper.h"
#include "CorePlatform/CCPointMath.h"
#include "InputHelper.h"


void NodeHelper::PauseAllNodes(CCNode* parentNode)
{
	CCArray_ForEach(CCNode*, child, parentNode->getChildren())
	{
		child->pauseSchedulerAndActions();
	}
	CCArray_ForEachEnd()
}

void NodeHelper::ResumeAllNodes(CCNode* parentNode)
{
	CCArray_ForEach(CCNode*, child, parentNode->getChildren())
	{
		child->resumeSchedulerAndActions();
	}
	CCArray_ForEachEnd()
}

void NodeHelper::PauseAllNodesRecoursive(CCNode* node)
{
	node->pauseSchedulerAndActions();

	CCArray_ForEach(CCNode*, child, node->getChildren())
	{
		PauseAllNodesRecoursive(child);
	}
	CCArray_ForEachEnd()
}

void NodeHelper::ResumeAllNodesRecoursive(CCNode* node)
{
	node->resumeSchedulerAndActions();

	CCArray_ForEach(CCNode*, child, node->getChildren())
	{
		ResumeAllNodesRecoursive(child);
	}
	CCArray_ForEachEnd()
}

void NodeHelper::CopyPropertiesFromNode(CCNode* source, CCNode* destination)
{
	destination->setScaleX(source->getScaleX());
	destination->setScaleY(source->getScaleY());
	destination->setRotation(source->getRotation());
}

void NodeHelper::AddToOtherNode(CCNode* parent, CCNode* node, int z)
{
	node->retain();
	{
		node->removeFromParentAndCleanup(true);
		if (z == -100)
			parent->addChild(node);
		else
			parent->addChild(node, z);
	}
	node->release();
}

//объединить(добавить) много нод в одну ноду resultNode, сделав anchorPoint у resultNode = 0.5
void NodeHelper::UnionNodesToOneNode(CCNode* resultNode, const vector<CCNode*>& nodesToUnion)
{
	//get content size of node
	CCRect rectSprites;
	for (uint j = 0; j < nodesToUnion.size(); ++j)
	{
		auto sprite = nodesToUnion[j];
		auto spriteBox = sprite->boundingBox();
		if (j == 0)
			rectSprites = spriteBox;
		else
			rectSprites = GeometryHelper::GetUnionRect(rectSprites, spriteBox);
	}

	resultNode->setAnchorPoint(ccp(0.5f, 0.5f));
	resultNode->setContentSize(rectSprites.size);
	resultNode->setPosition(ccp(rectSprites.getMidX(), rectSprites.getMidY()));

	for_each(auto sprite, nodesToUnion)
	{
		sprite->setPosition(sprite->getPosition() - rectSprites.origin);
		AddToOtherNode(resultNode, sprite);
	}
}

//изменить anchorPoint у ноды, Ќ≈ измен€€ позицию
void NodeHelper::ChangeNodeAnchorPoint(CCNode* sprite, const CCPoint& anchorPointNew)
{
	auto box = sprite->boundingBox();
	auto anchorPointOld = sprite->getAnchorPoint();
	auto oldPosition = sprite->getPosition();

	float factorX = sprite->getScaleX() < 0 ? -1 : 1;
	float factorY = sprite->getScaleY() < 0 ? -1 : 1;

	CCPoint newPosition = oldPosition - ccp(factorX*(anchorPointOld.x - anchorPointNew.x)*box.size.width,
		factorY*(anchorPointOld.y - anchorPointNew.y)*box.size.height);

	sprite->setAnchorPoint(anchorPointNew);
	sprite->setPosition(newPosition);
}

void NodeHelper::ChangeZ(CCNode* node, int z)
{
	node->getParent()->reorderChild(node, z);
}

bool NodeHelper::IsNodeContainsPoint(CCNode* sprite, const CCPoint& point)
{
	if (sprite == NULL || !sprite->isVisible())
		return false;

	CCRect rect = sprite->boundingBox();
#if COCOS2D_VERSION >= 0x00020000
	rect = GeometryHelper::CCRectMultiply(rect, CC_CONTENT_SCALE_FACTOR());
#endif

	bool isContains = rect.containsPoint(point);
	return isContains;
}

bool NodeHelper::IsNodeContainsTouchPoint(CCNode* node, CCSet* touches)
{
	auto touchPoint = InputHelper::GetTouchPointBySet(touches);
	auto nodePoint = node->getParent()->convertToNodeSpace(touchPoint);
	bool isContains = node->boundingBox().containsPoint(nodePoint);
	return isContains;
}