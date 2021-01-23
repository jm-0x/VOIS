workspace "ChatApplication"
    architecture "x86_64"
    configurations {"Debug", "Release"}
    defines{"ASIO_STANDALONE"}

    filter "system:windows"
        defines
        {
            "BOOST_DATE_TIME_NO_LIB",
            "BOOST_REGEX_NO_LIB",
            "_WIN32_WINNT=0x0601"
        
        }

 output = "%{cfg.system}/%{cfg.buildcfg}"

project "Client"
    kind "ConsoleApp"
    language "C++"
    location "Client"
    cppdialect "C++17"

    targetdir ("bin/%{prj.name}/" .. output)
    objdir("bin-obj/%{prj.name}/" .. output)

    files 
    {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
    }

    includedirs { "Dependencies/asio/include" }

project "Server"
    kind "ConsoleApp"
    language "C++"
    location "Server"
    cppdialect "C++17"

    targetdir ("bin/%{prj.name}/" .. output)
    objdir("bin-obj/%{prj.name}/" .. output)
    
    files 
    {
        "%{prj.location}/src/**.cpp",
        "%{prj.location}/src/**.h",
    }

    includedirs { "Dependencies/asio/include","Dependencies/spdlog/include" }