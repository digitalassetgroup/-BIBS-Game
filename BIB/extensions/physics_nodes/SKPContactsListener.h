#pragma once

#ifndef __SKP_CONTACTS__LISTENER___
#define __SKP_CONTACTS__LISTENER___

#include "Box2D/Dynamics/b2WorldCallbacks.h"
#include "Box2D/Common/b2Math.h"

class SKPContactsListener : public b2ContactListener
{
public:
	virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

#endif //#ifndef __SKP_CONTACTS__LISTENER___