set_languages("cxx17")

set_xmakever("2.5.1")

add_requires("tiltedcore", "mimalloc", "hopscotch-map", {configs = {rltgenrandom = true }})
add_requires("catch2")

add_rules("mode.debug","mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

if is_mode("release") then
    add_ldflags("/LTCG", "/OPT:REF")
    add_cxflags("/Ot", "/GL", "/Ob2", "/Oi", "/GS-")
    add_defines("NDEBUG")
    set_optimize("fastest")
end

target("TiltedReverse")
    set_kind("static")
    set_group("Libraries")
    add_includedirs(
        "Code/reverse/include/", 
        "ThirdParty/", 
        "/ThirdParty/disasm/", 
        "/ThirdParty/mhook/",
        {public = true})
    add_files("Code/reverse/src/*.cpp")
    add_headerfiles("Code/reverse/include/*.hpp", {prefixdir = "TiltedReverse"})
    add_packages("tiltedcore", "mimalloc", "hopscotch-map")

target("Loader")
    set_kind("binary")
    set_group("Libraries")
    add_includedirs(
        "Code/reverse/include/", 
        "Code/loader/include/",
        {public = true})
    add_files("Code/loader/src/*.cpp")
    add_headerfiles("Code/loader/include/*.hpp", {prefixdir = "Loader"})
    add_packages("tiltedcore", "mimalloc", "hopscotch-map")
    add_syslinks("user32", "shell32")

target("Proxy")
    set_kind("shared")
    set_group("Libraries")
    add_includedirs(
        "Code/reverse/include/", 
        "Code/loader/include/",
        {public = true})
    add_files(
        "Code/loader/src/Config.cpp",
        "Code/proxy/src/*.cpp")
    add_headerfiles("Code/loader/include/*.hpp", {prefixdir = "Loader"})
    add_packages("tiltedcore", "mimalloc", "hopscotch-map")

target("disasm")
    set_kind("static")
    set_group("Libraries")
    add_includedirs(
        "ThirdParty/disasm/",
        {public = true})
    add_files("ThirdParty/disasm/*.c")
    add_headerfiles("ThirdParty/disasm/*.h", {prefixdir = "disasm"})

target("mhook")
    set_kind("static")
    set_group("Libraries")
    add_includedirs(
        "ThirdParty/mhook/", 
        "ThirdParty/disasm/",
        {public = true})
    add_files("ThirdParty/mhook/*.cpp")
    add_headerfiles("ThirdParty/mhook/*.h", {prefixdir = "mhook"})
    add_links("disasm")

target("TiltedReverse_Tests")
    set_kind("binary")
    set_group("Tests")
    add_files("Code/tests/src/*.cpp")
    add_deps("TiltedReverse")
    add_packages("tiltedcore", "catch2", "hopscotch-map")
