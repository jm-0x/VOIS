workspace "ChatApplication"
    architecture "x86_64"
    configurations {"Debug", "Realse"}

project "Client"
    kind "ConsoleApp"
    language "C++"

project "Server"
    kind "ConsoleApp"
    language "C++"