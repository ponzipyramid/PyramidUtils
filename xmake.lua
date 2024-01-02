-- set minimum xmake version
set_xmakever("2.7.8")

-- set project
set_project("pyramid-utils")
set_version("0.0.0")
set_license("MIT")
set_languages("c++20")
set_optimize("faster")
set_warnings("allextra", "error")

-- set allowed
set_allowedarchs("windows|x64")
set_allowedmodes("debug", "releasedbg")

-- set defaults
set_defaultarchs("windows|x64")
set_defaultmode("releasedbg")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- require packages
add_requires("commonlibsse-ng", { configs = { skyrim_vr = false } })

-- targets
target("PyramidUtils")
    -- add packages to target
    add_packages("fmt", "spdlog", "commonlibsse-ng")

    -- add commonlibsse-ng plugin
    add_rules("@commonlibsse-ng/plugin", {
        name = "pyramid-utils",
        author = "ponzipyramid",
        description = "Simple Papyrus utils"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    after_build(function(target)
        local plugins = path.join("dist", "SKSE/Plugins")
        os.mkdir(plugins)
        os.trycp(target:targetfile(), plugins)
        os.trycp(target:symbolfile(), plugins)
    end)
