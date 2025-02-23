#include "registerTypes.h"

//godotcpp
#include <gdextension_interface.h>
#include <godot_cpp/classes/engine.hpp>

//own
#include "spotLight2D.h"
#include "beamLight2D.h"
#include "lightEnvironment2D.h"

void gdextension_initialize(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		ClassDB::register_class<SpotLight2D>();
		ClassDB::register_class<LightEnvironment2D>();
		ClassDB::register_class<BeamLight2D>();
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
