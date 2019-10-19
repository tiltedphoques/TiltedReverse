function CreateReverseProject(basePath, coreBasePath)
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

function CreateLoaderProject(basePath, coreBasePath)
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

function CreateDisasmProject(basePath)
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

function CreateMhookProject(basePath)
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

function LazyReverseProjects(basePath, coreBasePath)
    group ("Applications")
        CreateLoaderProject(basePath, coreBasePath)

    group ("Libraries")
        CreateReverseProject(basePath, coreBasePath)
        CreateCoreProject(coreBasePath)

     group("ThirdParty")
        CreateDisasmProject(basePath)
        CreateMhookProject(basePath)
end
