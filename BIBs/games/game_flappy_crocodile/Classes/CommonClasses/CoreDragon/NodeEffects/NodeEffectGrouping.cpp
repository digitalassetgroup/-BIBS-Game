#include "NodeEffectGrouping.h"
#include "Core/CollectionHelper.h"

NodeEffectGrouping::NodeEffectGrouping(cocos2d::CCNode& nodeToEffect)
	: INodeEffect(nodeToEffect), _isAboutToRemove(), _isComplete()
{
}

NodeEffectGrouping::~NodeEffectGrouping()
{
	for_each(auto effect, _initialEffects) {
		SAFE_DELETE(effect);
	}

	if (!_currentEffectsStack.empty())
	{
		for (auto it = _currentEffectsStack.begin(); it != _currentEffectsStack.end(); ++it) {
			SAFE_DELETE(*it);
		}
	}
}

NodeEffectGrouping* NodeEffectGrouping::clone() const
{
	//TODO: надо как-то сделать, чтобы вызывался метод Clone у всех парентов
	return NULL; //not implemented
}

void NodeEffectGrouping::AddEffect(INodeEffect* effect)
{
	_initialEffects.push_back(effect);
}

void NodeEffectGrouping::onStart()
{
	_isComplete = false;
	_currentEffectsStack.clear();

	for_each(auto effect, _initialEffects)
	{
		_currentEffectsStack.push_back(effect->clone()); 
	}

	std::reverse(_currentEffectsStack.begin(), _currentEffectsStack.end());
	NextEffect();
}

bool NodeEffectGrouping::isComplete() const
{
	return _isComplete;
}

void NodeEffectGrouping::NextEffect()
{
	auto effectToStart = _currentEffectsStack.back();
	effectToStart->setEffectNotifier(this);
	g_effectMgr->addGameEffect(_parentNode, effectToStart);	
	_currentEffectsStack.pop_back();
}

//IGameEffectNotifier
void NodeEffectGrouping::OnEffectStart(IGameEffect* effect)
{
}

void NodeEffectGrouping::OnEffectFinish(IGameEffect* effect)
{
	if (_isAboutToRemove)
		return;

	if (effect->GetLoopCount() == 0)
	{
		if (_currentEffectsStack.empty())
			_isComplete = true;
		else
			NextEffect();
	}
}

//
void NodeEffectGrouping::OnBeforeDestroy()
{
	_isAboutToRemove = true;
}

void NodeEffectGrouping::onUpdate(const float deltaTime)
{
}