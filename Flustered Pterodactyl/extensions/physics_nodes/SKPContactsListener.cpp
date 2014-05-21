#include "SKPContactsListener.h"
#include "CCPhysicsSprite.h"
#include "Box2D/Box2D.h"

USING_NS_CC_EXT;

void SKPContactsListener::BeginContact(b2Contact* contact)
{
	CCPhysicsSprite * a = (CCPhysicsSprite *) contact->GetFixtureA()->GetBody()->GetUserData();
	CCPhysicsSprite * b = (CCPhysicsSprite *) contact->GetFixtureB()->GetBody()->GetUserData();
	
	if (a && a->beginContactCallEnabled())
	{
		a->handleBeginContact(contact);
	}
	if (b && b->beginContactCallEnabled())
	{
		b->handleBeginContact(contact);
	}
}

void SKPContactsListener::EndContact(b2Contact* contact)
{
	CCPhysicsSprite * a = (CCPhysicsSprite *) contact->GetFixtureA()->GetBody()->GetUserData();
	CCPhysicsSprite * b = (CCPhysicsSprite *) contact->GetFixtureB()->GetBody()->GetUserData();
	
	if (a && a->endContactCallEnabled())
	{
		a->handleEndContact(contact);
	}
	if (b && b->endContactCallEnabled())
	{
		b->handleEndContact(contact);
	}
}

void SKPContactsListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	CCPhysicsSprite * a = (CCPhysicsSprite *) contact->GetFixtureA()->GetBody()->GetUserData();
	CCPhysicsSprite * b = (CCPhysicsSprite *) contact->GetFixtureB()->GetBody()->GetUserData();
	
	if (a && a->preContactCallEnabled())
	{
		a->handlePreSolve(contact, oldManifold);
	}
	if (b && b->preContactCallEnabled())
	{
		b->handlePreSolve(contact, oldManifold);
	}
}

void SKPContactsListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	CCPhysicsSprite * a = (CCPhysicsSprite *) contact->GetFixtureA()->GetBody()->GetUserData();
	CCPhysicsSprite * b = (CCPhysicsSprite *) contact->GetFixtureB()->GetBody()->GetUserData();
	
	if (a && a->postContactCallEnabled())
	{
		a->handlePostSolve(contact, impulse);
	}
	if (b && b->postContactCallEnabled())
	{
		b->handlePostSolve(contact, impulse);
	}
}

