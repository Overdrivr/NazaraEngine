kind "ConsoleApp"

files {"main.cpp","Generator.hpp","Generator.cpp"}

if (_OPTIONS["united"]) then
	configuration "DebugStatic"
		links "NazaraEngine-s-d"

	configuration "ReleaseStatic"
		links "NazaraEngine-s"

	configuration "DebugDLL"
		links "NazaraEngine-d"

	configuration "ReleaseDLL"
		links "NazaraEngine"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraRenderer-s-d"
		links "NazaraUtility-s-d"
		links "NazaraNoise-s-d"
		links "NazaraGraphics-s-d"
		links "NazaraNoise-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraRenderer-s"
		links "NazaraUtility-s"
		links "NazaraNoise-s"
		links "NazaraGraphics-s"
		links "NazaraNoise-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraRenderer-d"
		links "NazaraUtility-d"
		links "NazaraNoise-d"
		links "NazaraGraphics-d"
		links "NazaraNoise-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraRenderer"
		links "NazaraUtility"
		links "NazaraNoise"
		links "NazaraGraphics"
		links "NazaraNoise"
end
