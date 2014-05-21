#pragma once
#ifndef NodeScheduleLambda_h
#define NodeScheduleLambda_h

#include "Core/CoreAll.h"
#include <cocos2d.h>
#include "Core/ObserverEvent.h"
#include "CoreDragon/NodeEffects/CCCallLambda.h"
using namespace cocos2d;

enum { NodeAnimationTag = 10113 };

template<typename TFunc>
static CCCallLambda* NodeScheduleLambda(CCNode* parent, float countSeconds, const TFunc& func)
{
	auto action = CCCallLambda::create(countSeconds, func);
	parent->runAction(action);
	return action;

	/*
	struct ThisNode : CCNode
	{
		private_ TFunc _func;
		public_ ThisNode(TFunc& func) : _func(func) {}

		void Init(CCNode* parent, float countSeconds)
		{
			parent->addChild(this);
			this->setTag(NodeAnimationTag);
			this->release();
			schedule(schedule_selector(ThisNode::Done), countSeconds);
		}

		void Done(ccTime deltaTime)
		{
			unschedule(schedule_selector(ThisNode::Done));
			_func();
			this->removeFromParentAndCleanup(true);
		}
	};

	auto node = new ThisNode(func);
	node->Init(parent, countSeconds);
	*/

	//NodeSchedule(parent, countSeconds, &func, member);
}


template<typename TFuncUpdate, typename TFuncEnd>
static CCCallLambdaUpdate* NodeScheduleLambdaUpdate(CCNode* parent, const TFuncUpdate& funcUpdate, const TFuncEnd& funcEnd)
{
	//TODO: надо как-нибудь обойти тот момент, что CCSequence принимает только CCFiniteActionы... 
	//т.е. сделать свой CCSequence, который принимает CCActionы!
	auto action = CCCallLambdaUpdate::create(funcUpdate, funcEnd);
	parent->runAction(action);
	return action;

	/*
	struct ThisNode : CCNode
	{
		private_ TFuncUpdate _funcUpdate;
		private_ TFuncEnd _funcEnd;
		public_ ThisNode(TFuncUpdate& funcUpdate, TFuncEnd& funcEnd) 
			: _funcUpdate(funcUpdate), _funcEnd(funcEnd) {}

		void Init(CCNode* parent)
		{
			parent->addChild(this);
			this->setTag(NodeAnimationTag);
			this->release();
			schedule(schedule_selector(ThisNode::Update), 0);
		}

		void Update(ccTime dt)
		{
			bool isFinish = _funcUpdate();
			if (isFinish)
				Done();
		}

		void Done()
		{
			unschedule(schedule_selector(ThisNode::Update));
			_funcEnd();
			this->removeFromParentAndCleanup(true);
		}
	};

	auto node = new ThisNode(funcUpdate, funcEnd);
	node->Init(parent);
	*/
}


static void NodeScheduleLambdaRemove(CCNode* parent)
{
	parent->stopAllActions();

	/*
	CCNode* animationNode = NULL;
	while ((animationNode = parent->getChildByTag(NodeAnimationTag)) != NULL)
	{
		animationNode->removeFromParentAndCleanup(true);
	}
	*/
}

#endif
