#ifndef AI_H
#define AI_H

#include <deque>
#include <landscape.h>
#include <modules/engines/controls.h>
#include <modules/flight/flightinfo.h>
#include <modules/flight/autopilot.h>
#include <modules/math/Rendezvous.h>


struct Rating {
    Rating() : attack(0), defense(0),
            order(0), opportunity(0),
            necessity(0),
            danger(0) { }
    float attack;
    float defense;
    float order;
    float opportunity;
    float necessity;
    float danger;
};


struct Personality {
    Personality() : confidence(.25), obedience(.25),
            experience(.25), cautiousness(.25) { }
    float confidence;
    float obedience;
    float experience;
    float cautiousness;
    
    float evaluate(const Rating & r);
    void randomize();
};
        

class Idea : virtual public Object {
public:
    virtual Rating rate()=0;
    virtual void realize()=0;
    virtual void postpone()=0;
    virtual std::string info()=0;
};

class PatrolIdea : public Idea {
    AutoPilot * ap;
    FlightInfo * fi;
    Vector area;
    std::deque<Vector> path;
    Ptr<ITerrain> terrain;
    float radius;
public:
    PatrolIdea(
        AutoPilot *,
        FlightInfo *,
        const Vector & area,
        float radius,
        Ptr<ITerrain>);
    virtual Rating rate();
    virtual void realize();
    virtual void postpone();
    virtual std::string info();
    inline std::deque<Vector> & getPath() { return path; }
protected:
    void choosePath();
};

class AttackIdea : public Idea {
    AutoPilot *ap;
    FlightInfo *fi;
    Ptr<IActor> source;
    Ptr<IGame> thegame;
    Ptr<IActor> target;
    Rendezvous rendezvous;
    Vector p, v, front;
    Vector target_rendezvous;
    Ptr<DroneControls> controls;
    double last_target_select;
    double last_missile;
    double last_bullet;
    double delta_t;
    double target_angle;
    double target_dist;
public:
    AttackIdea(Ptr<IGame>,
               Ptr<IActor> source,
               AutoPilot *ap,
               FlightInfo *fi,
               Ptr<DroneControls> controls);
    virtual Rating rate();
    virtual void realize();
    virtual void postpone();
    virtual std::string info();
protected:
    void evaluateSituation();
    bool canFireMissile();
    void fireMissile();
    bool canFireBullet();
    void fireBullet();
    bool canSelectTarget();
    void selectTarget();
    void followTarget();
};

class EvadeTerrainIdea : public Idea {
    AutoPilot * ap;
    FlightInfo * fi;
public:
    EvadeTerrainIdea( AutoPilot *, FlightInfo * );
    virtual Rating rate();
    virtual void realize();
    virtual void postpone();
    virtual std::string info();
};

// 
// class ShootIdea : public Idea {
// public:
//     virtual Rating rate();
// 	virtual void realize();
// };

#endif
