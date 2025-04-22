#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct") # type: ignore
env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1 # whatever this is, is preventing a dumb error with scons

# If you include any external library, define it here along with its headers
# It'll be used for both sources and linkage
usesLibrary = None

# The output name of your program
outputDir = "demo/addons/msiafterburner/"
outputLibName = "msiafterburner"

## Begin ##

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags
env.Append(CPPPATH=["src/"], CCFLAGS=["-O2"])
sources = Glob("src/*.cpp") # type: ignore

# Link the static library
static_lib_ext = ".a"

match env["platform"]:
    case "windows":
        static_lib_ext = ".lib"
    case "android":
        pass
    case "macos":
        pass
    case "ios":
        pass

lib = None
if usesLibrary != None:
    try:
        lib = Glob("lib/{}/{}/lib{}.{}.{}".format(usesLibrary, env["platform"], usesLibrary, env["arch"], static_lib_ext))[0]
    except:
        print("\033[91m", end="\r")
        raise FileNotFoundError("\033[1mNo existing static library found for {}.{}\033[0m".format(env["platform"], env["arch"]))

    print("\033[94mSelected static lib: \033[1m{}\033[0m".format(str(lib)))
    sources.append(lib)

# Compile for different platforms:

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{}{}.{}.{}.framework/lib{}.{}.{}".format(
            outputDir, outputLibName, env["platform"], env["target"], usesLibrary, env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "{}{}.{}.{}.simulator.a".format(outputDir, outputLibName, env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "{}{}.{}.{}.a".format(outputDir, outputLibName, env["platform"], env["target"]),
            source=sources,
        )
else:
    # Android and else
    library = env.SharedLibrary(
        "{}{}{}{}".format(outputDir, outputLibName, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)