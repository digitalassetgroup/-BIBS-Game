#include "SKPInput.h"
#include "SKPHero.h"
#include "Box2D/Box2D.h"


SKPHero::SKPHero()
:
_acceleration(1.0f),
_maxVelocity(8.0f),
_jumpHeight(11.0f),
_jumpAcceleration(0.3f),
_killVelocity(3.0f),
_enemySpringHeight(8.0f),
_enemySpringJumpHeight(9.0f),
_hurtDuration(1000.0f),
_hurtVelocityX(6.0f),
_hurtVelocityY(10.0f),
_canDuck(false),
_input(0),
_onGround(false),
_springOffEnemy(-1),
_hurtTimeoutID(0),
_hurt(false),
_friction(0.75f),
_playerMovingHero(false),
_controlsEnabled(true),
_ducking(false),
_combinedGroundAngle(0)
{
	_preContactCallEnabled   = true;
	_beginContactCallEnabled = true;
	_endContactCallEnabled   = true;
	//scheduleUpdate();
}

SKPHero* SKPHero::create(const char *pszFileName)
{
    SKPHero* pRet = new SKPHero();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

void SKPHero::setControlsEnabled(bool enabled)
{
	_controlsEnabled = enabled;
	if (!_controlsEnabled)
	{
		setFriction(_friction);
	}
}

void SKPHero::defineBody(b2BodyDef * bodyDef)
{
	CCPhysicsSprite::defineBody(bodyDef);
	bodyDef->fixedRotation = true;
	bodyDef->allowSleep    = false;
}

void SKPHero::setFriction(float friction)
{
	if (_friction != friction)
	{
		_friction = friction;
		setFixturesFriction(_friction);
	}
}

void SKPHero::setFixturesFriction(float friction)
{
	if (m_pB2Body)
	{
		for (b2Fixture * f = m_pB2Body->GetFixtureList(); f; f = f->GetNext())
		{
			f->SetFriction(friction);
		}
	}
}

void SKPHero::defineFixture(b2FixtureDef * fixtureDef)
{
	CCPhysicsSprite::defineFixture(fixtureDef);
	fixtureDef->friction            = _friction;
	fixtureDef->restitution         = 0;
	fixtureDef->filter.categoryBits = SKP_GoodGuys;
}

b2Vec2 SKPHero::getSlopeBasedMoveAngle() const
{
	return CCPhysicsSprite::Rotateb2Vec2(b2Vec2(_acceleration, 0), _combinedGroundAngle);
}

void SKPHero::update(float dt)
{
	if (!m_pB2Body)
	{
		return;
	}
	b2Vec2 velocity = m_pB2Body->GetLinearVelocity();	
	if (_controlsEnabled && _input)
	{
		bool moveKeyPressed = false;
		_ducking = _canDuck && _onGround && _input->isDoing(SKPICDuck);
		if (!_ducking && _input->isDoing(SKPICRight))
		{
			velocity.x += _acceleration * 60 * dt;
			//getSlopeBasedMoveAngle();
			moveKeyPressed = true;
		}
		if (!_ducking && _input->isDoing(SKPICLeft))
		{
			velocity.x -= _acceleration * 60 * dt;
			//getSlopeBasedMoveAngle();нет
			moveKeyPressed = true;
		}
		//If player just started moving the hero this tick.
		if (moveKeyPressed && !_playerMovingHero)
		{
			_playerMovingHero = true;
			setFixturesFriction(0);
		}
		else
		//Player just stopped moving the hero this tick.			
		if (!moveKeyPressed && _playerMovingHero)
		{
			_playerMovingHero = false;
			//Add friction so that he stops running
			setFixturesFriction(_friction);

			velocity.x = 0;
		}
		if (_onGround && !_ducking && (_input->justDid(SKPICJump) || _input->isDoing(SKPICJump)))
		//if (fabs(velocity.y) < _jumpHeight * 0.05)
		{
			velocity.y = _jumpHeight;
			setFixturesFriction(0);
			//onJump.dispatch();
		}
		if (!_onGround && velocity.y > 0 && _input->isDoing(SKPICJump))
		{
			velocity.y += _jumpAcceleration * 60 * dt;
		}
		if (_springOffEnemy != -1)
		{
			if (_input->isDoing(SKPICJump))
			{
				velocity.y = -_enemySpringJumpHeight;
			}
			else
			{
				velocity.y = -_enemySpringHeight;
			}
			_springOffEnemy = -1;
		}

	}
	if (velocity.x > _maxVelocity)
	{
		velocity.x = _maxVelocity;
	}
	else
	if (velocity.x < -_maxVelocity)
	{
		velocity.x = -_maxVelocity;
	}
	
	if (!_onGround)
	{
		velocity.y -= 1.6 * 60 * dt;
	}
	
	//gravity
	m_pB2Body->SetLinearVelocity(velocity);
	updateAnimations();
}

void SKPHero::updateAnimations()
{
	
}

void SKPHero::_addGroundContact(b2Fixture * fixture)
{
	int ind = _indexGroundContact(fixture);
	if (-1 != ind)
	{
		CCLOGWARN("SKPHero::_addGroundContact : contact already added");
	//	return;
	}
	_groundContacts.push_back(fixture);
}

bool SKPHero::_removeGroundContact(b2Fixture * fixture)
{
	int ind = _indexGroundContact(fixture);
	if (-1 == ind)
	{
		return false;
	}
	else
	{
		if (ind != _groundContacts.size() - 1)
		{
			_groundContacts[ind] = _groundContacts.back();
		}
		_groundContacts.pop_back();
		return true;
	}
}

int SKPHero::_indexGroundContact(b2Fixture * fixture)
{
	for (int i = 0; i < _groundContacts.size(); ++i)
	{
		if (_groundContacts[i] == fixture)
		{
			return i;
		}
	}
	return -1;
}

void SKPHero::handleBeginContact(b2Contact* contact)
{
	CCPhysicsSprite * collider = getOther(contact);
	if (!collider)
	{
		return;
	}
	// TODO : enemies
	
	if (collider->skptype() != SKPT_Sensor)
	{
		b2Vec2 localPt = contact->GetManifold()->localPoint;
		printf("localPt.x = %f localPt.y = %f\n", localPt.x, localPt.y);
		if (localPt.x != 0 || localPt.y != 0) // find out is this valid sense
		{
			b2Vec2 normal = CCPhysicsSprite::getWorldNormal(contact);
			float collisionAngle;
			if (normal.x == 0)
			{
				if (normal.y > 0)
				{
					collisionAngle = 90;
				}
				else
				{
					collisionAngle = 270;
				}
			}
			else
			{
				collisionAngle = CC_RADIANS_TO_DEGREES(b2Atan2(normal.y, normal.x));
			}
			
			if (collisionAngle < 0)
			{
				collisionAngle += 360;
			}
			else
			if (collisionAngle > 360)
			{
				collisionAngle -= 360;
			}
			
			if(! (collisionAngle >=0 && collisionAngle <= 360))
			{
				printf("collisionAngle : %f\n", collisionAngle);
				CCAssert(false, "incorrect angle\n");
			}
							printf("collisionAngle : %f\n", collisionAngle);
			if (collisionAngle > 45 && collisionAngle < 135)
			{
				_addGroundContact(collider->getB2Body()->GetFixtureList()); // check if this is correct
				if (!_onGround)
				{
					_onGround = true;
					if (!_playerMovingHero)
					{
						setFixturesFriction(_friction);
					}
				}

				CCLOGWARN("_onGround = true");
				updateCombinedGroundAngle();
			}
			
		}
	}
}

void SKPHero::updateCombinedGroundAngle()
{
	_combinedGroundAngle = 0;
	if (!_groundContacts.size())
	{
		return;
	}
	for (int i = 0; i < _groundContacts.size(); ++i)
	{
		float angle = CC_RADIANS_TO_DEGREES(_groundContacts[i]->GetBody()->GetAngle());
		int a = int(angle) % 45;
		_combinedGroundAngle += a;		
	}
	_combinedGroundAngle /= float(_groundContacts.size());
}

void SKPHero::handleEndContact(b2Contact* contact)
{
	CCPhysicsSprite * collider = getOther(contact);
	if (_removeGroundContact(collider->getB2Body()->GetFixtureList()))
	{
		if (_groundContacts.size() == 0)
		{
			_onGround = false;
			CCLOGWARN("_onGround = false");
		}
		updateCombinedGroundAngle();
	}
}

void SKPHero::handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	if (!_ducking)
	{
		return;
	}
	CCPhysicsSprite * other = getOther(contact);
	if (!other)
	{
		return;
	}
	assert(false); // should be implemented
}































