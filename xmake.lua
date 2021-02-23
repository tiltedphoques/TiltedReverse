set_languages("cxx17")

set_xmakever("2.5.1")

add_requires("tiltedcore", "hopscotch-map", "minhook", "catch2")

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
    add_includedirs("Code/reverse/include/", {public = true})
    add_files("Code/reverse/src/*.cpp")
    add_headerfiles("Code/reverse/include/*.hpp", {prefixdir = "TiltedReverse"})
    add_packages("tiltedcore", "mimalloc", "hopscotch-map", "minhook")

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

target("TiltedReverse_Tests")
    set_kind("binary")
    set_group("Tests")
    add_files("Code/tests/src/*.cpp")
    add_deps("TiltedReverse")
    add_packages("tiltedcore", "catch2", "hopscotch-map", "minhook")
