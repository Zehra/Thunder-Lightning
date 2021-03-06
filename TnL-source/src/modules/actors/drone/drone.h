#ifndef DRONE_H
#define DRONE_H

#include <tnl.h>
#include <modules/math/Vector.h>
#include <modules/math/Matrix.h>
#include <modules/model/model.h>
#include <modules/model/Skeleton.h>
#include <modules/flight/flightinfo.h>
#include <modules/engines/controls.h>
#include <modules/actors/simpleactor.h>
#include <modules/collide/CollisionManager.h>
#include <modules/weaponsys/Armament.h>
#include <modules/scripting/IoIncludes.h>


class DroneCockpit;
class RigidEngine;
class Targeter;
class SoundSource;
struct Context;
class RenderPass;
struct ICamera;

namespace Effectors { class Wheel; class TailHook; }

class EventSheet;

class Drone : public SimpleActor, public Collide::Collidable, virtual public SigObject {
public:
    Drone(Ptr<IGame> thegame, IoObject * io_peer=0);
    virtual ~Drone();
    
    void init();

    virtual void onLinked();
    virtual void onUnlinked();
    
    virtual void action();
    virtual void kill();

    virtual void draw();

    virtual void applyDamage(float damage, int domain, Ptr<IProjectile>);
    virtual float getRelativeDamage();

    virtual void integrate(float delta_t, Transform * transforms);
    virtual void update(float delta_t, const Transform * new_transforms);
    virtual void collide(const Collide::Contact & c);
    
    virtual int getNumViews();
    virtual Ptr<IView> getView(int n);

    void explode(bool deadly=true);

    virtual bool hasControlMode(ControlMode);
    virtual void setControlMode(ControlMode);
    
    void setLandingGear(bool lowered);
    inline bool isLandingGearLowered() { return gear_lowered; }
    inline void toggleLandingGear() { flight_controls->setGearLowered(!flight_controls->isGearLowered()); }
    
    void setLandingHook(bool lowered);
    inline bool isHookGearLowered() { return hook_lowered; }
    inline void toggleLandingHook() { flight_controls->setHookLowered(!flight_controls->isHookLowered()); }
    
private:
    void updateDerivedObjects();
    void drawWheels();
    
private:
    JRenderer * renderer;
    Ptr<ITerrain> terrain;
    Ptr<SoundSource> engine_sound_src;
    Ptr<DroneCockpit> cockpit;

    // 3d model
    Ptr<Model> wheel_model;

    // flight stuff
    Ptr<RigidEngine> engine;
    FlightInfo flight_info;
    Ptr<FlightControls> flight_controls;
    bool gear_lowered, hook_lowered;
    Ptr<Effectors::Wheel> wheels[3];
    Ptr<Effectors::TailHook> hook;
    float engine_power;

    float damage;
};




#endif
