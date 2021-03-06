#include <algorithm>
#include <tnl.h>
#include <modules/physics/RigidBody.h>

#include "Collidable.h"
#include "Contact.h"

void Collide::Contact::applyCollisionImpulse() {
    ls_message("applyCollisionImpulse begin.\n");
    Ptr<RigidBody> rigid_a = collidables[0]->getRigid();
    Ptr<RigidBody> rigid_b = collidables[1]->getRigid();

    if (!rigid_a && !rigid_b) return;
    
    const float e = 1.0f; // elasticity
    
    if (!rigid_a) {
        float j = RigidBody::collisionImpulseMagnitude( e, *rigid_b, p, -n);
        rigid_b->applyImpulseAt( -j * n, p);
        ls_message("rigid_b mass: %f velocity: ", rigid_b->getBase().M); rigid_b->getLinearVelocity().dump();
    } else if (!rigid_b) {
        float j = RigidBody::collisionImpulseMagnitude( e, *rigid_a, p, n);
        rigid_a->applyImpulseAt( j * n, p);
        ls_message("rigid_a mass: %f velocity: ", rigid_a->getBase().M); rigid_a->getLinearVelocity().dump();
    } else {
        // both rigid_a and rigid_b are valid, we have a "real" collision
        float j = RigidBody::collisionImpulseMagnitude(
            e, *rigid_a, *rigid_b, p, n);
        //ls_message("Applying impulse of magnitude %f\n", j);
        //ls_message("in direction %1.3f %1.3f %1.3f\n",
        //    n[0],n[1],n[2]);

        rigid_a->applyImpulseAt( j * n, p);
        rigid_b->applyImpulseAt(-j * n, p);
        ls_message("rigid_a mass: %f velocity: ", rigid_a->getBase().M); rigid_a->getLinearVelocity().dump();
        ls_message("rigid_b mass: %f velocity: ", rigid_b->getBase().M); rigid_b->getLinearVelocity().dump();
    }
    ls_message("applyCollisionImpulse end.\n");
}

void Collide::Contact::swap() {
    std::swap(collidables[0], collidables[1]);
    std::swap(domains[0], domains[1]);
    n = -n;
    std::swap(v[0], v[1]);
}

