local projectname = "Tutorial-16b_AssemblyFiles"
project (projectname)
	dependson { "External" }
	
	location "%{dirs.projectfiles}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	verify_or_create_settings(projectname)

	files {
		"source/**.h",
		"source/**.hpp",
		"source/**.cpp",
	}

	includedirs {
		"../../Engine",
		"../TutorialCommon",
		"../../External",
		"source/**"
	}

	libdirs { 
		"%{dirs.dependencies}",
		"%{dirs.lib}"
	}

	links {
		"External", "Engine"
	}

	defines {"_CONSOLE"}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"

	systemversion "latest"
	
	filter "system:windows"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		defines {
			"WIN32",
			"_LIB", 
			"TGE_SYSTEM_WINDOWS"
		}