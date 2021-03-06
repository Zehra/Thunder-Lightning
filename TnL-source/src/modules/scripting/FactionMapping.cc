// ---------------------------------------------------------------
// |  FactionMapping                                             |
// ---------------------------------------------------------------

#include <Faction.h>
#include "mappings.h"

namespace {
	
	struct FactionMapping : public TemplatedObjectMapping<Faction> {
        static const char *const id;
        
		static void addMapping(Ptr<IGame> thegame, IoState * state) {
			IoObject *lobby = state->lobby;
			
			IoObject *self = proto(state);
			IoState_registerProtoWithId_(state, self, id);
			IoObject_setSlot_to_(lobby, IOSYMBOL("Faction"), self);
		}
		
		TAG_FUNC
		
		static IoObject *proto(void *state) {
			IoMethodTable methodTable[] = {
				{"getName", getName},
				{"setName", setName},
				{"getDefaultAttitude", getDefaultAttitude},
				{"setDefaultAttitude", setDefaultAttitude},
				{"getAttitudeTowards", getAttitudeTowards},
				{"setAttitudeTowards", setAttitudeTowards},
				{"color", getColor},
				{"setColor", setColor},
				{NULL, NULL}
			};
			IoObject *self = IoObject_new(state);
			IoObject_tag_(self, tag(state, "Faction"));
			IoObject_setDataPointer_(self, 0);
			retarget(self, new Faction);
			IoObject_addMethodTable_(self, methodTable);
			return self;
		}
	
		static IoObject * create(Ptr<Faction> faction, IoState *state) 
		{
            IoState_pushCollectorPause(state);
			IoObject *child = IoObject_rawClone(
				IoState_protoWithId_(state, id));
			retarget(child, ptr(faction));
            IoState_addValueIfNecessary_(state, child);
            IoState_popCollectorPause(state);
			return child;
		}
			
		static IoObject * rawClone(IoObject *self) 
		{ 
			IoObject *child = IoObject_rawClonePrimitive(self);
			retarget(child, new Faction);
			*getObject(child) = *getObject(self);
			return child;
		}
		
		static IoObject * getAttitudeTowards
		(IoObject *self, IoObject *locals, IoMessage *m) {
			BEGIN_FUNC("Faction.getAttitudeTowards")
			IOASSERT(IoMessage_argCount(m) == 1,"Expected one argument")
			Ptr<Faction> other = getObject(
				IoMessage_locals_valueArgAt_(m, locals, 0));
			return wrapObject<int>(
				getObject(self)->getAttitudeTowards(other), IOSTATE);
		}

		static IoObject * setAttitudeTowards
		(IoObject *self, IoObject *locals, IoMessage *m) {
			BEGIN_FUNC("Faction.setAttitudeTowards")
			IOASSERT(IoMessage_argCount(m) == 2,"Expected two arguments")
			Ptr<Faction> other = getObject(
				IoMessage_locals_valueArgAt_(m, locals, 0));
			int attitude = unwrapObject<int> (
				IoMessage_locals_valueArgAt_(m, locals, 1));
			getObject(self)->setAttitudeTowards(
				other,
				(Faction::Attitude) attitude);
			return self;
		}
		
		GET_STRING(getName)
		SET_STRING(setName)
		GET_NUMBER(getDefaultAttitude)
		SET_ENUM(setDefaultAttitude, Faction::Attitude)
		GET_VECTOR(getColor)
		SET_VECTOR(setColor)
	};
    
    const char *const FactionMapping::id = "Faction";
}

template<>
void addMapping<Faction>(Ptr<IGame> game, IoState *state) {
	FactionMapping::addMapping(game,state);
}

template<>
IoObject * 
wrapObject<Ptr<Faction> >(Ptr<Faction> faction, IoState * state) {
	return FactionMapping::create(faction, state);
}

template<>
Ptr<Faction> unwrapObject(IoObject * self) {
    return FactionMapping::getObject(self);
}


template<>
IoObject *getProtoObject<Ptr<Faction> >(IoState * state) {
	return IoState_protoWithId_(state, FactionMapping::id);
}
