if (not _OPTIONS["united"]) then
	project "NazaraTerrainRenderer"
end

files
{
	"../include/Nazara/TerrainRenderer/**.hpp",
	"../include/Nazara/TerrainRenderer/**.inl",
	"../src/Nazara/TerrainRenderer/**.hpp",
	"../src/Nazara/TerrainRenderer/**.cpp"
}

if (os.is("windows")) then
	excludes { "../src/Nazara/TerrainRenderer/Posix/*.hpp", "../src/Nazara/TerrainRenderer/Posix/*.cpp" }
else
	excludes { "../src/Nazara/TerrainRenderer/Win32/*.hpp", "../src/Nazara/TerrainRenderer/Win32/*.cpp" }
end

if (_OPTIONS["united"]) then
	excludes "../src/Nazara/TerrainRenderer/Debug/Leaks.cpp"
else
	configuration "DebugStatic"
		links "NazaraCore-s-d"
		links "NazaraUtility-s-d"
		links "NazaraRenderer-s-d"
		links "NazaraGraphics-s-d"

	configuration "ReleaseStatic"
		links "NazaraCore-s"
		links "NazaraUtility-s"
		links "NazaraRenderer-s"
		links "NazaraGraphics-s"

	configuration "DebugDLL"
		links "NazaraCore-d"
		links "NazaraUtility-d"
		links "NazaraRenderer-d"
		links "NazaraGraphics-d"

	configuration "ReleaseDLL"
		links "NazaraCore"
		links "NazaraUtility"
		links "NazaraRenderer"
		links "NazaraGraphics"
end