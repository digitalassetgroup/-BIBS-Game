#pragma once

#ifndef __SKP_HERO___
#define __SKP_HERO___

#include "CCPhysicsSprite.h"
#include <vector>

USING_NS_CC;
USING_NS_CC_EXT;

class SKPInput;
class b2Fixture;
class b2Contact;
class b2Manifold;

class SKPHero : public CCPhysicsSprite
{
public:
	SKPHero();
	
    static SKPHero * create(const char *pszFileName);
	
	virtual void defineBody(b2BodyDef * bodyDef);
	virtual void defineFixture(b2FixtureDef * fixtureDef);
	
	inline float friction() const
	{
		return _friction;
	}
	void  setFriction(float friction);
	void  setFixturesFriction(float friction);
	// Whether or not the player can move and jump with the hero
	inline bool controlsEnabled() const
	{
		return _controlsEnabled;
	}
	void setControlsEnabled(bool enabled);
	// Returns true if the hero is on the ground and can jump.
	inline bool onGround() const
	{
		return _onGround;
	}
	void update(float dt);
	inline void setInput(SKPInput * input)
	{
		_input = input;
	}
	inline void setCanDuck(bool canDuck)
	{
		_canDuck = canDuck;
	}
	
	inline void setJumpHeight(float jumpHeight)
	{
		_jumpHeight = jumpHeight;
	}
	
	inline void setJumpAcceleration(float jumpAcceleration)
	{
		_jumpAcceleration = jumpAcceleration;
	}
	
	inline void setMaxVelocity(float maxVelocity)
	{
		_maxVelocity = maxVelocity;
	}
	
	b2Vec2 getSlopeBasedMoveAngle() const;
	
	virtual void handleBeginContact(b2Contact* contact);
    virtual void handleEndContact(b2Contact* contact);
    virtual void handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	
private:
	void updateAnimations();
	void updateCombinedGroundAngle();
private:
	void    _addGroundContact(b2Fixture * fixture);
	bool _removeGroundContact(b2Fixture * fixture);
	int   _indexGroundContact(b2Fixture * fixture);
	
	float _friction;
	float _acceleration;
	float _maxVelocity;
	float _jumpHeight;
	float _jumpAcceleration;
	float _killVelocity;
	float _enemySpringHeight;
	float _enemySpringJumpHeight;
	float _hurtDuration;
	float _hurtVelocityX;
	float _hurtVelocityY;
	bool  _canDuck;
	SKPInput * _input;
	std::vector<b2Fixture *> _groundContacts;
	bool _onGround;
	float _springOffEnemy;
	int _hurtTimeoutID;
	bool _hurt;
	bool _playerMovingHero;
	bool _controlsEnabled;
	bool _ducking;
	float _combinedGroundAngle;
	
};





#endif //#ifndef __SKP_HERO___