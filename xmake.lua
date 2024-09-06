set_languages("cxx17")
set_arch("x64")

set_xmakever("2.6.0")

-- direct dependency version pinning
add_requires(
    "tiltedcore v0.2.7", 
    "hopscotch-map v2.3.1", 
    "minhook v1.3.3", 
    "catch2 2.13.9", 
    "mem 1.0.0",
    "xbyak v7.06"
)

-- dependencies' dependencies version pinning
add_requireconfs("*.mimalloc", { version = "2.1.7", override = true })
add_requireconfs("*.cmake", { version = "3.30.2", override = true })

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
        {public = true})
    add_files("Code/reverse/src/*.cpp")
    add_headerfiles(
        "Code/reverse/include/*.hpp",
        "Code/reverse/include/*.inl",  
        {prefixdir = "TiltedReverse"})
    add_defines("NOMINMAX") -- stupid ass windows
    add_packages("tiltedcore", "mimalloc", "hopscotch-map", "minhook", "mem", "xbyak")

target("TiltedReverse_Tests")
    set_kind("binary")
    set_group("Tests")
    add_files("Code/tests/src/*.cpp")
    add_deps("TiltedReverse")
    add_packages("tiltedcore", "catch2", "hopscotch-map", "minhook", "xbyak")
