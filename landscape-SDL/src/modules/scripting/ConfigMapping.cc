// ---------------------------------------------------------------
// |  ConfigMapping                                              |
// ---------------------------------------------------------------

#include <interfaces/IConfig.h>
#include "mappings.h"

namespace {
	
	struct ConfigMapping : public TemplatedObjectMapping<IConfig> {
		static void addMapping(Ptr<IGame> thegame, IoState * state) {
			IoObject *object = state->mainActor;
			IoObject *lobby = state->lobby;
			
			IoObject *config = proto(state);
			IoState_registerProtoWithFunc_(state, config, proto);
			IoObject_setSlot_to_(lobby,
				IoState_stringWithCString_(state, "Config"), config);
			retarget(config, &*thegame->getConfig());
		}
		
		static IoObject *proto(void *state) {
			IoMethodTable methodTable[] = {
				/* standard I/O */
				{"query", query},
				{"set", set},
				{NULL, NULL}
			};
			IoObject *self = IoObject_new(state);
			self->tag = tag(state, "Config");
			self->data = 0;
			IoObject_addMethodTable_(self, methodTable);
			return self;
		}
	
		static IoObject * query
		(IoObject *self, IoObject *locals, IoMessage *m) {
			BEGIN_FUNC("Config.query")
			IOASSERT(IoMessage_argCount(m) == 1,"Expected one argument")
			char * key = IoMessage_locals_cStringArgAt_(m, locals, 0);
			char valuebuf[256];
			const char *val = getObject(self)->query(key);
			if (val) {
				strncpy(valuebuf, val, 256);
				return IOSTRING(valuebuf);
			} else {
				return IONIL(self);
			}
		}
		
		static IoObject * set
		(IoObject *self, IoObject *locals, IoMessage *m) {
			BEGIN_FUNC("Config.set")
			IOASSERT(IoMessage_argCount(m) == 2,"Expected two arguments")
			char * key = IoMessage_locals_cStringArgAt_(m, locals, 0);
			char * val = IoMessage_locals_cStringArgAt_(m, locals, 1);
			getObject(self)->set(key,val);
			return self;
		}
	};
}

template<>
void addMapping<IConfig>(Ptr<IGame> game, IoState *state) {
	ConfigMapping::addMapping(game,state);
}

template<>
IoObject * 
wrapObject<Ptr<IConfig> >(Ptr<IConfig> config, IoState * state) {
	IoObject *new_object = ConfigMapping::rawClone(
		IoState_protoWithInitFunction_(state, ConfigMapping::proto));
	ConfigMapping::retarget(new_object, &*config);
	return new_object;
}

template<>
Ptr<IConfig> unwrapObject<Ptr<IConfig> >(IoObject * self) {
	return (IConfig*)self->data;
}
