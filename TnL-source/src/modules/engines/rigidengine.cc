#include <tnl.h>
#include <algorithm>
#include "rigidengine.h"
#include <modules/clock/clock.h>
#include <DataNode.h>


namespace {
    void dump_state(const RigidBodyState & s, const char* indent) {
        ls_message("%sx: ",indent); s.x.dump();
        ls_message("%sP: ",indent); s.P.dump();
        ls_message("%sq: %.2f   ",indent, s.q.real()); s.q.imag().dump();
        ls_message("%sL: ",indent); s.L.dump();
    }
} // namespace

RigidEngine::RigidEngine(Ptr<IGame> thegame)
:   thegame(thegame)
{
    ::RigidBodyState state = {
        Vector(0,0,0),
        Quaternion(1, Vector(0,0,0)),
        Vector(0,0,0),
        Vector(0,0,0)};
    setState(state);
    clearForces();
    setControls(new DataNode);
};

void RigidEngine::setControls(Ptr<DataNode> controls) {
    this->controls = controls;
}

void RigidEngine::clearAndApplyEffectors() {
    clearForces();
    // Apply all physical effects from effectors
    for(Effectors::iterator i=effectors.begin(); i!=effectors.end(); ++i) {
        (*i)->applyEffect(*this, getControls());
    }
}

void RigidEngine::run() {
}


// IPositionProvider
Vector RigidEngine::getLocation() { return getState().x; }
Vector RigidEngine::getFrontVector() { return getState().q.rot(Vector(0,0,1)); }
Vector RigidEngine::getRightVector() { return getState().q.rot(Vector(1,0,0)); }
Vector RigidEngine::getUpVector()    { return getState().q.rot(Vector(0,1,0)); }
void RigidEngine::getOrientation(Vector *up, Vector *right, Vector *front) {
    *up = getState().q.rot(Vector(0,1,0));
    *right = getState().q.rot(Vector(1,0,0));
    *front = getState().q.rot(Vector(0,0,1));
}

// IMovementProvider
Vector RigidEngine::getMovementVector() { return getLinearVelocity(); }

// IPositionReceiver
void RigidEngine::setLocation(const Vector & new_p) {
    ::RigidBodyState state = getState();
    state.x = new_p;
    setState(state);
}

#define ls_vector(v) (v)[0], (v)[1], (v)[2]

void RigidEngine::setOrientation( const Vector & up,
                                  const Vector & right,
                                  const Vector & front) {
    /*
    ls_message("<RigidEngine::setOrientation>\n");
    ls_message("Requested: %+1.3f %+1.3f %+1.3f right\n"
               "           %+1.3f %+1.3f %+1.3f up\n"
               "           %+1.3f %+1.3f %+1.3f front\n",
               ls_vector(right), ls_vector(up), ls_vector(front));
    */

    ::RigidBodyState state = getState();
    state.q.fromMatrix(MatrixFromColumns(right,up,front));
    setState(state);
    /*
    ls_message("Got:       %+1.3f %+1.3f %+1.3f right\n"
               "           %+1.3f %+1.3f %+1.3f up\n"
               "           %+1.3f %+1.3f %+1.3f front\n",
               ls_vector(state.q.rot(Vector(1,0,0))),
               ls_vector(state.q.rot(Vector(0,1,0))),
               ls_vector(state.q.rot(Vector(0,0,1))));
    ls_message("</RigidEngine::setOrientation>\n");
    */
}

// IMovementReceiver
void RigidEngine::setMovementVector(const Vector & new_v) {
    ::RigidBodyState state = getState();
    state.P = Vector(0,0,0);
    setState(state);
    applyLinearVelocity(new_v);
}


void RigidEngine::integrate(float delta_t, Transform * transforms) {
    //ls_message("RigidEngine::integrate() of body with mass %.2f\n", getBase().M);
    //ls_message("  state(t=0):\n");
    //dump_state(getState(), "    ");
    
    // shortcut to query current state
    if (delta_t == 0) {
        const ::RigidBodyState & state = getState();
        transforms[0] = Transform(state.q.normalize(), state.x);
        return;
    }
        

    clearAndApplyEffectors();
    ::RigidBodyState y = getState();
    ::RigidBodyState k1 = getDerivative();
    
    ::RigidBodyState y_k1 = y;
    y_k1.x += 0.5f*delta_t * k1.x;
    y_k1.q = (y_k1.q + 0.5f*delta_t * k1.q).normalize();
    y_k1.P += 0.5f*delta_t * k1.P;
    y_k1.L += 0.5f*delta_t * k1.L;
    setState(y_k1);
    clearAndApplyEffectors();
    ::RigidBodyState k2 = getDerivative();
    
    ::RigidBodyState y_k2 = y;
    y_k2.x += 0.5f*delta_t * k2.x;
    y_k2.q = (y_k2.q + 0.5f*delta_t * k2.q).normalize();
    y_k2.P += 0.5f*delta_t * k2.P;
    y_k2.L += 0.5f*delta_t * k2.L;
    setState(y_k2);
    clearAndApplyEffectors();
    ::RigidBodyState k3 = getDerivative();
    
    ::RigidBodyState y_k3 = y;
    y_k3.x += delta_t * k3.x;
    y_k3.q = (y_k3.q + delta_t * k3.q).normalize();
    y_k3.P += delta_t * k3.P;
    y_k3.L += delta_t * k3.L;
    setState(y_k3);
    clearAndApplyEffectors();
    ::RigidBodyState k4 = getDerivative();
    
    ::RigidBodyState result = y;
    result.x += delta_t / 6 * (k1.x + 2*k2.x + 2*k3.x + k4.x);
    result.q = (result.q + delta_t / 6 * (k1.q + 2.0f*k2.q + 2.0f*k3.q + k4.q)).normalize();
    
    //ls_message("  state(t=%.2f):\n", delta_t);
    //dump_state(result, "    ");
    
    transforms[0] = Transform(result.q.normalize(), result.x);
    
    setState(y);
    
    //s_message("  state(t=0 again):\n");
    //dump_state(getState(), "    ");
}

void RigidEngine::update(float delta_t, const Transform * new_transforms) {
    //ls_message("RigidEngine::update() of body with mass %.2f\n", getBase().M);
    //ls_message("  state(t=0):\n");
    //dump_state(getState(), "    ");

    clearAndApplyEffectors();
    ::RigidBodyState y = getState();
    ::RigidBodyState k1 = getDerivative();
    
    ::RigidBodyState y_k1 = y;
    y_k1.x += 0.5f*delta_t * k1.x;
    y_k1.q = (y_k1.q + 0.5f*delta_t * k1.q).normalize();
    y_k1.P += 0.5f*delta_t * k1.P;
    y_k1.L += 0.5f*delta_t * k1.L;
    setState(y_k1);
    clearAndApplyEffectors();
    ::RigidBodyState k2 = getDerivative();
    
    ::RigidBodyState y_k2 = y;
    y_k2.x += 0.5f*delta_t * k2.x;
    y_k2.q = (y_k2.q + 0.5f*delta_t * k2.q).normalize();
    y_k2.P += 0.5f*delta_t * k2.P;
    y_k2.L += 0.5f*delta_t * k2.L;
    setState(y_k2);
    clearAndApplyEffectors();
    ::RigidBodyState k3 = getDerivative();
    
    ::RigidBodyState y_k3 = y;
    y_k3.x += delta_t * k3.x;
    y_k3.q = (y_k3.q + delta_t * k3.q).normalize();
    y_k3.P += delta_t * k3.P;
    y_k3.L += delta_t * k3.L;
    setState(y_k3);
    clearAndApplyEffectors();
    ::RigidBodyState k4 = getDerivative();
    
    ::RigidBodyState result = y;
    result.x = new_transforms[0].vec();
    //result.q = (result.q + delta_t / 6 * (k1.q + 2.0f*k2.q + 2.0f*k3.q + k4.q)).normalize();
    result.q = new_transforms[0].quat();
    result.P += delta_t / 6 * (k1.P + 2*k2.P + 2*k3.P + k4.P);
    result.L += delta_t / 6 * (k1.L + 2*k2.L + 2*k3.L + k4.L);
    
    setState(result);

    //ls_message("  state(t=%.3f):\n", delta_t);
    //dump_state(getState(), "    ");
}

void RigidEngine::addEffector(Ptr<IEffector> effector) {
    effectors.push_back(effector);
}

void RigidEngine::removeEffector(Ptr<IEffector> effector) {
    Effectors::iterator i = find(effectors.begin(), effectors.end(), effector);
    if (i != effectors.end()) {
        effectors.erase(i);
    }
}

