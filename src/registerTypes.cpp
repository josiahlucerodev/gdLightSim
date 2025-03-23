#include "registerTypes.h"

//godotcpp
#include <gdextension_interface.h>
#include <godot_cpp/classes/engine.hpp>

//own
#include "light/emitter/spotLight2D.h"
#include "light/emitter/beamLight2D.h"
#include "light/emitter/circleLight2D.h"
#include "light/interactor/mirror2D.h"
#include "light/interactor/lens2D.h"
#include "light/interactor/colorFilter2D.h"
#include "light/interactor/lightCollider2D.h"
#include "light/interactor/lightSensor2D.h"
#include "light/lightEnvironment2D.h"

void gdextension_initialize(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<LightEnvironment2D>();

		//emitters
		ClassDB::register_class<SpotLight2D>();
		ClassDB::register_class<BeamLight2D>();
		ClassDB::register_class<CircleLight2D>();
		
		//interactors
		ClassDB::register_class<ColorFilter2D>();
		ClassDB::register_class<Lens2D>();
		ClassDB::register_class<Mirror2D>();
		ClassDB::register_class<LightColider2D>();
		ClassDB::register_class<LightSensor2D>();
	}
}

void gdextension_terminate(ModuleInitializationLevel p_level) {
}

extern "C"
{
	GDExtensionBool GDE_EXPORT gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(gdextension_initialize);
		init_obj.register_terminator(gdextension_terminate);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
