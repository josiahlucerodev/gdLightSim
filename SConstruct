#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path

# TODO: Do not copy environment after godot-cpp/test is updated <https://github.com/godotengine/godot-cpp/blob/master/test/SConstruct>.
env = SConscript("godot-cpp/SConstruct")

# Add source files.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# Find gdextension path even if the directory or extension is renamed 
(extension_path,) = glob("project/addons/*/*.gdextension")

# Get the addon path 
addon_path = Path(extension_path).parent

# Get the project name from the gdextension file 
project_name = Path(extension_path).stem

scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("Scons cache enabled... (path: '" + scons_cache_path + "')")

# Create the library target 
debug_or_release = "release" if env["target"] == "template_release" else "debug"
if env["platform"] == "macos":
    arch = os.uname()[4]
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.{4}.framework/{1}.{2}.{3}.{4}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            arch
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

Default(library)
