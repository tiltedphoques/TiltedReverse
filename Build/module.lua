function CreateReverseProject(basePath, coreBasePath)
    project ("Reverse")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            basePath .. "/Code/reverse/include/",
            coreBasePath .. "/Code/core/include/",
        }

        files
        {
            basePath .. "/Code/reverse/include/**.h",
            basePath .. "/Code/reverse/src/**.cpp",
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
