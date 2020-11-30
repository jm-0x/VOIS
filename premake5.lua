workspace "ChatApplication"
    architecture "x86_64"
    configurations {"Debug", "Realse"}

 output = "%{cfg.system}/%{cfg.buildcfg}"

project "Client"
    kind "ConsoleApp"
    language "C++"
    location "Client"

    targetdir ("bin/%{prj.name}/" .. output)
    objdir("bin-obj/%{prj.name}/" .. output)

    files 
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
    }

    includedirs { "vendors/asio" }

project "Server"
    kind "ConsoleApp"
    language "C++"
    location "Server"

    targetdir ("bin/%{prj.name}/" .. output)
    objdir("bin-obj/%{prj.name}/" .. output)
    
    files 
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
    }

    includedirs { "vendors/asio" }