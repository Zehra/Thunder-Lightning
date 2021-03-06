#include <interfaces/IActor.h>
#include <debug.h>
#include "Targeter.h"
#include "RadarNet.h"

#define SECONDS_TILL_REVERIFICATION 2
#define SECONDS_LOST_TILL_REMOVAL   5
#define OPERATIONS_PER_SECOND       20

/// Global set of radar networks definition
std::set<RadarNet*> &RadarNet::getRadarNets() {
    static std::set<RadarNet*> *_radar_nets=0;
    
    if (!_radar_nets) {
        _radar_nets = new std::set<RadarNet*>;
    }
    
    return *_radar_nets;
}

RadarNet::Enumerator::Enumerator(Ptr<RadarNet> rn) {
    radarnet = rn;
    iter = radarnet->all_contacts.end();
    toBegin();
}

RadarNet::Enumerator::Enumerator(Ptr<RadarNet> rn, RadarNet::ContactsIter i) {
    radarnet = rn;
    iter = i;
    if (!atEnd()) {
        actor = (*iter)->actor.lock();
        ++(*iter)->usecount;
    }
}

RadarNet::Enumerator::Enumerator(const RadarNet::Enumerator & other) {
    radarnet = other.radarnet;
    iter = other.iter;
    actor = other.actor;
    if (!atEnd()) ++(*iter)->usecount;
}

RadarNet::Enumerator::~Enumerator() {
    if (!atEnd()) --(*iter)->usecount;
}

void RadarNet::Enumerator::next(bool backward) {
    do {
        advance(backward);
    } while ( (!actor || isZombie()) && !atEnd() );
}

void RadarNet::Enumerator::cycle(bool backward) {
    Enumerator old = *this;
    next(backward);
    if (atEnd()) {
        toBegin(backward);
    }
    while ( !atEnd() && !(actor = (*iter)->actor.lock()) ) {
        advance(backward);
    }
}

bool RadarNet::Enumerator::atEnd() const { return iter == radarnet->all_contacts.end(); }
Ptr<IActor> RadarNet::Enumerator::getActor() const { return (isValid() && actor->isAlive())?actor:Ptr<IActor>(); }
Ptr<IActor> RadarNet::Enumerator::getActorRaw() const { return actor; }
bool RadarNet::Enumerator::isValid() const { return !atEnd() && !isZombie() && actor; }
bool RadarNet::Enumerator::isVerified() const { return (*iter)->state == Contact::VERIFIED; }
bool RadarNet::Enumerator::isLost() const { return (*iter)->state == Contact::LOST; }
bool RadarNet::Enumerator::isZombie() const { return (*iter)->state == Contact::ZOMBIE; }
float RadarNet::Enumerator::ageOfInformation() const { return (*iter)->age; }
Vector RadarNet::Enumerator::lastKnownPosition() const { return (*iter)->position; }
bool RadarNet::Enumerator::operator ==(const RadarNet::Enumerator & other) {
    return other.iter == iter;
}
bool RadarNet::Enumerator::operator !=(const RadarNet::Enumerator & other) {
    return other.iter != iter;
}
const RadarNet::Enumerator & RadarNet::Enumerator::operator =(const RadarNet::Enumerator & other) {
    if (!atEnd()) --(*iter)->usecount;
    radarnet = other.radarnet;
    iter = other.iter;
    actor = other.actor;
    if (!atEnd()) ++(*iter)->usecount;
    return *this;
}

void RadarNet::Enumerator::advance(bool backward) {
    if (atEnd()) return;
    
    --(*iter)->usecount;
    if (backward) {
        if (iter == radarnet->all_contacts.begin()) {
            iter = radarnet->all_contacts.end();
        } else {
            --iter;
        }
    } else {
        ++iter;
    }
    if (atEnd()) {
        actor = 0;
    } else {
        actor = (*iter)->actor.lock();
        ++(*iter)->usecount;
    }
}

void RadarNet::Enumerator::toBegin(bool backward) {
    if (!atEnd()) --(*iter)->usecount;
    if ( radarnet->all_contacts.empty() ) {
        iter = radarnet->all_contacts.end();
        actor = 0;
        return;
    }
    if (backward) {
        iter = --radarnet->all_contacts.end();
    } else {
        iter = radarnet->all_contacts.begin();
    }
    
    actor = (*iter)->actor.lock();
    ++(*iter)->usecount;
    if (!actor || isZombie()) {
        next(backward);
    }
}

void RadarNet::Enumerator::toEnd() {
    if (!atEnd()) --(*iter)->usecount;
    actor = 0;
    iter = radarnet->all_contacts.end();
}   

RadarNet::RadarNet() {
    all_iter = all_contacts.begin();
    
    getRadarNets().insert(this);
}

RadarNet::~RadarNet() {
    getRadarNets().erase(this);
}

void RadarNet::updateAllRadarNets(float delta_t) {
    typedef std::set<RadarNet*>::iterator Iter;
    for(Iter i= getRadarNets().begin(); i!= getRadarNets().end(); ++i) {
        (*i)->update(delta_t);
    }
}

RadarNet::Enumerator RadarNet::getEnumeratorForActor(Ptr<IActor> actor) {
    ContactsByActor::iterator i = contacts_by_actor.find(actor);
    if (i == contacts_by_actor.end()) {
        return Enumerator(this, all_contacts.end());
    } else {
        return Enumerator(this, i->second->all_iter);
    }
}

void RadarNet::reportPossibleContact(Ptr<IActor> actor, Ptr<Targeter> witness) {
    ContactsByActor::iterator i = contacts_by_actor.find(actor);

    if (i == contacts_by_actor.end()) {
        // This is a new, i.e. formerly unreported possible radar contact.
        // If the LOS test succeeds, we can add it to verified_contacts
        // and all_contacts
        if (witness->hasLineOfSightTo(actor)) {
            Ptr<Contact> contact = new Contact;
            contacts_by_actor[actor] = contact;
            contact->actor      = actor;
            contact->witness    = witness;
            contact->position   = actor->getLocation();
            contact->age        = 0;
            contact->usecount   = 0;
            contact->state      = Contact::VERIFIED;
            
            all_contacts.push_back(contact);
            contact->all_iter   = --all_contacts.end();
        }
    } else {
        // We already know this contact. If it is a lost or zombie contact, perform a
        // LOS test. If that succeeds, we can mark this contact as verified
        // again.
        Ptr<Contact> contact = i->second;
        if (contact->state != Contact::VERIFIED && witness->hasLineOfSightTo(actor)) {
            contact->witness = witness;
            contact->age = 0;
            contact->state = Contact::VERIFIED;
        }
    }
}

void RadarNet::reportSelf(Ptr<Targeter> witness) {
    Ptr<IActor> actor = &witness->getSubjectActor();
    ContactsByActor::iterator i = contacts_by_actor.find(actor);

    if (i == contacts_by_actor.end()) {
        // This is the first time the contact reports itself
        
        Ptr<Contact> contact = new Contact;
        contacts_by_actor[actor] = contact;
        contact->actor      = actor;
        contact->witness    = witness;
        contact->position   = actor->getLocation();
        contact->age        = 0;
        contact->usecount   = 0;
        contact->state      = Contact::VERIFIED;
        
        all_contacts.push_back(contact);
        contact->all_iter   = --all_contacts.end();
    } else {
        // We already know this contact. Just refresh it.
        Ptr<Contact> contact = i->second;
        contact->witness = witness;
        contact->age = 0;
        contact->state = Contact::VERIFIED;
    }
}

void RadarNet::update(float delta_t) {
    for(ContactsIter i= all_contacts.begin(); i!= all_contacts.end(); ++i) {
        (*i)->age += delta_t;
    }
    
    size_t num_ops = (size_t) (0.9999f + OPERATIONS_PER_SECOND*delta_t);
    num_ops = std::min(num_ops, all_contacts.size());
    
    // perform the calculated number of contact operations
    while(num_ops-- > 0) {
        if (all_iter == all_contacts.end()) {
            all_iter = all_contacts.begin();
        }
        
        // contacts whose actor has died are removed
        Ptr<IActor> actor = (*all_iter)->actor.lock();
        if ( !actor || !actor->isAlive()) {
            (*all_iter)->state = Contact::ZOMBIE;
        }
        
        // verified contacts old enough are reverified
        if ((*all_iter)->state == Contact::VERIFIED &&
            (*all_iter)->age > SECONDS_TILL_REVERIFICATION)
        {
            verifyContact();
        }
        
        // lost contacts old enough are removed
        if ((*all_iter)->state == Contact::LOST &&
            (*all_iter)->age > SECONDS_LOST_TILL_REMOVAL)
        {
            (*all_iter)->state = Contact::ZOMBIE;
        }
        
        // zombie contacts with zero usecount are erased from the list
        if ((*all_iter)->state == Contact::ZOMBIE &&
            (*all_iter)->usecount == 0)
        {
            contacts_by_actor.erase((*all_iter)->actor);
            all_contacts.erase(all_iter++);
        } else
        {
            ++all_iter;
        }
    }
}

void RadarNet::verifyContact() {
    Ptr<Contact> contact = *all_iter;
    Ptr<IActor> actor = contact->actor.lock();
    Ptr<Targeter> witness = contact->witness.lock();
    
    if (!actor) return;
    
    if (witness && witness->getSubjectActor().isAlive() && witness->hasLineOfSightTo(actor)) {
        contact->age = 0;
    } else {
        // this is now a lost contact
        contact->witness = 0;
        contact->age = 0;
        contact->state = Contact::LOST;
    }
}

