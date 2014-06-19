
#include "effect_manager.h"

#include "../utilities/ig_defines.h"
#include "../utilities/ig_assert.h"
#include "Core/CollectionHelper.h"
#include "CorePlatform/LogHelper.h"

#include "cocos2d.h"

using namespace cocos2d;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IGameEffectNotifier::~IGameEffectNotifier(void)
{
}

void IGameEffectNotifier::OnEffectStart(IGameEffect* gameEffect)
{
}

void IGameEffectNotifier::OnEffectFinish(IGameEffect* gameEffect)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IGameEffect::IGameEffect(void) :
	m_effectNotifier(nullptr), _isPaused(), _loopCount(1), _parentNode(), _isEffectFinishingNow(), _isRemoving(),
	_tag()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IGameEffect::~IGameEffect(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IGameEffect::onStart(void)
{
	if(nullptr != m_effectNotifier)
	{
		m_effectNotifier->OnEffectStart(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IGameEffect::onFinish(void)
{
	if(nullptr != m_effectNotifier)
	{
		m_effectNotifier->OnEffectFinish(this);
	}

	if (_loopCount == 0)
	{
		_isEffectFinishingNow = true;
		_funcAfterEffectFinish();
		_funcAfterEffectFinish.Clear();
		_isEffectFinishingNow = false;
	}
}

void IGameEffect::setEffectNotifier(IGameEffectNotifier* const effectNotifier) 
{ 
	m_effectNotifier = effectNotifier; 
}

void IGameEffect::Pause() 
{ 
	_isPaused = true; 
}

void IGameEffect::Resume() 
{ 
	_isPaused = false; 
}

bool IGameEffect::IsPaused() 
{ 
	return _isPaused; 
}

IGameEffect* IGameEffect::SetLoopCount(int loopCount) 
{ 
	_loopCount = loopCount; return this; 
}

int IGameEffect::GetLoopCount() const 
{ 
	return _loopCount; 
}

void IGameEffect::DoAfterEffectFinishedClear()
{
	_funcAfterEffectFinish.Clear();
}

void IGameEffect::SetTag( int tag )
{
	_tag = tag;
}

int IGameEffect::GetTag()
{
	return _tag;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INodeEffect::INodeEffect(cocos2d::CCNode& nodeToEffect) :
	m_node(nodeToEffect)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISpriteEffect::ISpriteEffect(cocos2d::CCSprite& spriteToEffect) :
	INodeEffect(spriteToEffect),
	m_sprite(spriteToEffect)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEffectManager* g_effectMgr(nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEffectManager* const CEffectManager::createInstance(void)
{
	assert_if(nullptr != g_effectMgr, "Instance of the Effect Manager already exists.  Use g_effectMgr.");
	
	g_effectMgr = new CEffectManager();
	return g_effectMgr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEffectManager::destroyInstance(void)
{
    //baad assert
	//assert_if(nullptr == g_effectMgr, "Instance of the Effect Manager does not exist at this time.");
    SAFE_DELETE(g_effectMgr)
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEffectManager::CEffectManager(void)
	: _isUpdating()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEffectManager::~CEffectManager(void)
{
	clearAllEffects(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEffectManager::clearAllEffects(bool isToFinish)
{
	_effectsToRemove.clear();

	while (!m_gameEffects.empty())
	{
		if (isToFinish)
		{
			(m_gameEffects.back())->onFinish();
		}
        auto effectToRemove = m_gameEffects.back();
		SAFE_DELETE(effectToRemove);
		m_gameEffects.pop_back();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IGameEffect* CEffectManager::addGameEffect(CCNode* parentNode, IGameEffect* const effectToAdd)
{
	effectToAdd->_parentNode = parentNode;
	assert_if(nullptr == effectToAdd, "Invalid parameter; effectToAdd - expected non-null pointer to a IGameEffect.");
	effectToAdd->onStart();
	_effectsToAdd.push_back(effectToAdd);
	return effectToAdd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//INodeEffect* CEffectManager::addNodeEffect(cocos2d::CCNode& nodeToEffect, const GameEffectType& effectType)
//{	
//	IG_UNUSED(nodeToEffect);
//	INodeEffect* newEffect(nullptr);
//	
//	switch (effectType)
//	{
//	case NODE_EFFECT_MOVE_TO:
//			
//	default:
//		assert_if(true, "Invalid value for parameter; effectType(%d) - out of range for a node effect.", effectType);			
//	};
//	
//	addGameEffect(newEffect);
//	return newEffect;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//ISpriteEffect* CEffectManager::addSpriteEffect(cocos2d::CCSprite& spriteToEffect, const GameEffectType& effectType)
//{
//	IG_UNUSED(spriteToEffect);
//	ISpriteEffect* newEffect(nullptr);
//	
//	switch (effectType)
//	{
//		case NODE_EFFECT_MOVE_TO:
//			
//		default:
//			assert_if(true, "Invalid value for parameter; effectType(%d) - out of range for a sprite effect.", effectType);			
//	};
//	
//	addGameEffect(newEffect);	
//	return newEffect;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEffectManager::update(const float deltaTime)
{
	_isUpdating = true;

	if (!_effectsToRemove.empty())
		RemoveEffectsToRemove();

	if (!_effectsToAdd.empty())
	{
		std::copy(_effectsToAdd.begin(), _effectsToAdd.end(), std::back_inserter(m_gameEffects));
		_effectsToAdd.clear();
	}

	for (auto it = m_gameEffects.begin(), itEnd = m_gameEffects.end(); it != itEnd; ++it)
	{
		auto effect = *it;
		
		if (effect->_isRemoving)
			continue;

		if (effect->isComplete())
		{
			if (effect->_loopCount > 0)
				--effect->_loopCount;

			effect->onFinish();
			if (effect->_isRemoving)
				continue;

			if (effect->_loopCount > 0 || effect->_loopCount == -1) //infinity
			{
				effect->onStart();
			}
			else
			{
				_effectsToRemove.push_back(effect);
				effect->_isRemoving = true;
				continue;
			}
		}
		
		if (!effect->IsPaused())
			effect->onUpdate(deltaTime);
	}

	_isUpdating = false;
}

//TODO: need to test!
void CEffectManager::PauseEffectsWithParent(cocos2d::CCNode* parentNode)
{
	for_each(IGameEffect* effect, m_gameEffects)
	{
		if (effect->_parentNode == parentNode)
			effect->Pause();
	}
}

void CEffectManager::ResumeEffectsWithParent(cocos2d::CCNode* parentNode)
{
	for_each(IGameEffect* effect, m_gameEffects)
	{
		if (effect->_parentNode == parentNode)
			effect->Resume();
	}
}

bool CEffectManager::IsHasEffectOfParent(cocos2d::CCNode* parentNode)
{
	for_each(IGameEffect* effect, m_gameEffects)
	{
		if (effect->_parentNode == parentNode)
			return true;
	}

	return false;
}

void CEffectManager::RemoveEffect(IGameEffect* effect, bool isToFinish)
{
	if (effect == nullptr)
		return;

	//---remove from _effectsToAdd----
	auto itFind = std::find(_effectsToAdd.begin(), _effectsToAdd.end(), effect);
	if (itFind != _effectsToAdd.end())
	{
		SAFE_DELETE(effect);
		_effectsToAdd.erase(itFind);
        return;
	}

	//---added to _effectsToRemove----
	effect->OnBeforeDestroy();
	_effectsToRemove.push_back(effect);
	effect->_isRemoving = true;
	if (isToFinish)
		effect->onFinish();
}
	
void CEffectManager::RemoveEffectsWithParent(cocos2d::CCNode* parentNode, bool isToFinish)
{
	//---remove from _effectsToAdd----
	//TODO: вынести этот алгоритм в CollectionHelper - RemoveIf with lambda
	for (auto it = _effectsToAdd.begin(); it != _effectsToAdd.end(); )
	{
		auto effect = *it;
		if (effect->_parentNode == parentNode)
		{
			SAFE_DELETE(effect);
			it = _effectsToAdd.erase(it);
			continue;
		}
		
		++it;
	}

	//---added to _effectsToRemove----
	_effectsTempRemove.clear(); //effects to remove

	for (auto it = m_gameEffects.begin(); it != m_gameEffects.end(); ++it)
	{
		auto effect = *it;

		if (effect->_parentNode == parentNode && !effect->_isEffectFinishingNow && !effect->_isRemoving)
		{
			effect->OnBeforeDestroy();
			_effectsTempRemove.push_back(effect);
			effect->_isRemoving = true;
		}
	}

	//удаляем ТОЛЬКО после того, как у эффектов вызван метод onFinish
	//т.к. у нас могут быть групповые эффекты, которые объединяют более мелкие эффекты
	//и эти мелкие эффекты при onFinish могут обращаться к групповым эффектам для нотификации 
	//(так делает класс NodeEffectGrouping)

	if (isToFinish)
	{
		for (auto it = _effectsTempRemove.begin(); it != _effectsTempRemove.end(); ++it)
		{		
			(*it)->onFinish();
		}
	}

	std::copy(_effectsTempRemove.begin(), _effectsTempRemove.end(), std::back_inserter(_effectsToRemove));
}

void CEffectManager::RemoveEffectsToRemove()
{
#ifdef DEBUG
	auto forDebug = _effectsToRemove;
#endif

	while (!_effectsToRemove.empty())
	{
		bool isRemoved = false;
		for (auto i = 0u; i < m_gameEffects.size(); ++i)
		{
			if (m_gameEffects[i] == _effectsToRemove.back())
			{
				m_gameEffects[i] = m_gameEffects.back();
				m_gameEffects.pop_back();
				isRemoved = true;
				break;
			}
		}

		MY_ASSERT(isRemoved, "");
        auto effectToRemove = _effectsToRemove.back();
		SAFE_DELETE(effectToRemove);
		_effectsToRemove.pop_back();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
