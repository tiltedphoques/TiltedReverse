premake.extensions.reverse = {}

function reverse_parent_path()
    local str = debug.getinfo(2, "S").source:sub(2)
    local dir =  str:match("(.*/)"):sub(0,-2)
    local index = string.find(dir, "/[^/]*$")
    return dir:sub(0, index)
end

function mhook_generate()

    local basePath = premake.extensions.reverse.path

    project "mhook"
        language "C++"
        kind "StaticLib"
        targetname "mhook"

        includedirs
        {
            basePath .. "/ThirdParty/mhook/",
            basePath .. "/ThirdParty/disasm/",
        }

        files
        {
            basePath .. "/ThirdParty/mhook/**.h",
            basePath .. "/ThirdParty/mhook/**.cpp",
        }

        links
        {
            "disasm"
        }

end

function disasm_generate(basePath)

    local basePath = premake.extensions.reverse.path

    project "disasm"
        language "C"
        kind "StaticLib"
        targetname "disasm"

        includedirs
        {
            basePath .. "/ThirdParty/disasm/",
        }

        files
        {
            basePath .. "/ThirdParty/disasm/**.h",
            basePath .. "/ThirdParty/disasm/**.c",
        }
end

function loader_generate(basePath, coreBasePath)

    local basePath = premake.extensions.reverse.path
    local coreBasePath = premake.extensions.core.path

    project ("Loader")
        kind ("WindowedApp")
        language ("C++")

        includedirs
        {
            basePath .. "/Code/reverse/include/",
            basePath .. "/Code/loader/include/",
            coreBasePath .. "/Code/core/include/",
        }

        files
        {
            basePath .. "/Code/loader/include/**.hpp",
            basePath .. "/Code/loader/src/**.cpp",
        }

        links
        {
            "Core"
        }

    project ("Proxy")
        kind ("SharedLib")
        language ("C++")

        includedirs
        {
            basePath .. "/Code/reverse/include/",
            basePath .. "/Code/proxy/include/",
            basePath .. "/Code/loader/include/",
            coreBasePath .. "/Code/core/include/",
        }

        files
        {
            basePath .. "/Code/loader/src/Config.cpp",
            basePath .. "/Code/proxy/include/**.hpp",
            basePath .. "/Code/proxy/src/**.cpp",
        }

        links
        {
            "Core"
        }
end

function reverse_generate()

    local basePath = premake.extensions.reverse.path
    local coreBasePath = premake.extensions.core.path    

    project ("Reverse")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            basePath .. "/Code/reverse/include/",
            coreBasePath .. "/Code/core/include/",
            basePath .. "/ThirdParty/mhook/",
            basePath .. "/ThirdParty/",
            basePath .. "/ThirdParty/disasm/",
        }

        files
        {
            basePath .. "/Code/reverse/include/**.hpp",
            basePath .. "/Code/reverse/src/**.cpp",
        }
end

function reverse_generate_all()
    if premake.extensions.reverse.generated == true then
        return
    end

    group ("Applications")
        loader_generate()

    group ("ThirdParty")
        mhook_generate()
        disasm_generate()

    group ("Libraries")
        reverse_generate()
        premake.extensions.core.generate()

    premake.extensions.reverse.generated = true
end

premake.extensions.reverse.path = reverse_parent_path()
premake.extensions.reverse.generate = reverse_generate_all