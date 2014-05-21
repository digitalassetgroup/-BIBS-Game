#pragma once
#ifndef _DracoGames_EffectManager_h_
#define _DracoGames_EffectManager_h_

#include <vector>
#include "Core/QFunction.h"

namespace cocos2d { class CCNode; class CCSprite; };

typedef enum GameEffectTypeEnum
{
	/* Effects for cocos2d::CCNode's */
	NODE_EFFECT_START = 100,
	NODE_EFFECT_MOVE_TO = NODE_EFFECT_START,
	NODE_EFFECT_END,
	
	/* Effects for cocos2d::CCSprite's */
	SPRITE_EFFECT_START = 200,
	SPRITE_EFFECT_FADE_IN = SPRITE_EFFECT_START,	/* planned not created */
	SPRITE_EFFECT_FADE_OUT,							/* planned not created */
	SPRITE_EFFECT_MAX,

	/* Effects for other things can be added... */
	
} GameEffectType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IGameEffect;

class IGameEffectNotifier
{
public:
	virtual ~IGameEffectNotifier(void);
	virtual void OnEffectStart(IGameEffect* gameEffect);
	virtual void OnEffectFinish(IGameEffect* gameEffect);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CEffectManager;

class IGameEffect
{
public:
	friend class CEffectManager;

	virtual ~IGameEffect(void);
	
	virtual void onStart(void);
	virtual void onFinish(void);
	virtual void OnBeforeDestroy() {}
	
	virtual void onUpdate(const float deltaTime) = 0;
	virtual bool isComplete(void) const = 0;
	
	void setEffectNotifier(IGameEffectNotifier* const effectNotifier);

	void Pause();
	void Resume();
	bool IsPaused();

	IGameEffect* SetLoopCount(int loopCount);	
	int GetLoopCount() const;

	void SetTag(int tag);
	int GetTag();

	template<typename TFunc> 
	void DoAfterEffectFinished(const TFunc& func) { _funcAfterEffectFinish = func; }
	void DoAfterEffectFinishedClear();

protected:
	IGameEffect(void);
	cocos2d::CCNode* _parentNode; //this field used only for pause and resume feature	

private:
	/* Disabled for protection, as many GameEffects will hold an object. */
	IGameEffect(const IGameEffect& obj);
	IGameEffect& operator=(const IGameEffect& obj);
	
	QFunction<void()> _funcAfterEffectFinish;	
	IGameEffectNotifier* m_effectNotifier;
	bool _isPaused;
	int _loopCount; //-1 - infinity, 1 - default
	bool _isEffectFinishingNow; //вызывается ли сейчас _funcAfterEffectFinish
	bool _isRemoving;
	int _tag;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class INodeEffect : public IGameEffect
{
public:
	INodeEffect(cocos2d::CCNode& nodeToEffect);
	
	INodeEffect* SetLoopCount(int loopCount) { IGameEffect::SetLoopCount(loopCount); return this; }

	//this method need by NodeEffectGrouping
	virtual INodeEffect* clone() const = 0;

protected:
	cocos2d::CCNode& m_node;
	
private:
	INodeEffect(const INodeEffect& obj);
	INodeEffect& operator=(const INodeEffect& obj);	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ISpriteEffect : public INodeEffect
{
public:
	ISpriteEffect(cocos2d::CCSprite& spriteToEffect);
	
protected:
	cocos2d::CCSprite& m_sprite;
	
private:
	ISpriteEffect(const ISpriteEffect& obj);
	ISpriteEffect& operator=(const ISpriteEffect& obj);	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector<IGameEffect*> EffectContainer;
typedef EffectContainer::iterator EffectItr;

class CEffectManager 
{
public:

	static CEffectManager* const createInstance(void);
	static void destroyInstance(void);
	
	void clearAllEffects(bool isToFinish);
	IGameEffect* addGameEffect(cocos2d::CCNode* parentNode, IGameEffect* const effectToAdd);
//	INodeEffect* addNodeEffect(cocos2d::CCNode& nodeToEffect, const GameEffectType& effectType);
//	ISpriteEffect* addSpriteEffect(cocos2d::CCSprite& spriteToEffect, const GameEffectType& effectType);
	
	void update(const float deltaTime);	

	void PauseEffectsWithParent(cocos2d::CCNode* parentNode);
	void ResumeEffectsWithParent(cocos2d::CCNode* parentNode);
	void RemoveEffectsWithParent(cocos2d::CCNode* parentNode, bool isToFinish);
	void RemoveEffect(IGameEffect* effect, bool isToFinish);
	bool IsHasEffectOfParent(cocos2d::CCNode* parentNode);

private:
	/* Prevent accidental use */
	CEffectManager(void);
	CEffectManager(const CEffectManager& obj);
	~CEffectManager(void);	
	CEffectManager& operator=(const CEffectManager& obj);
	void RemoveEffectsToRemove();	

	EffectContainer m_gameEffects;
	
	//temp containers
	EffectContainer _effectsToAdd;
	EffectContainer _effectsToRemove;
	EffectContainer _effectsTempRemove;
	bool _isUpdating;
};

extern CEffectManager* g_effectMgr;

#endif /* _DracoGames_EffectManager_h_ */
