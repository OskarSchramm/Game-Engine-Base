require('vstudio')

premake.api.register {
    name = "sdlchecks",
    scope = "config",
    kind = "boolean",
    default = true
}

-- needed a way to set SDLChecks which isn't provided in premake
-- might be better ways to do it, right now I just opted to push it in
-- where it is in the project settings.

premake.override(premake.vstudio.vc2010, "multiProcessorCompilation", function(base, prj)
	premake.w('<SDLCheck>' .. tostring(true) .. '</SDLCheck>')
	base(prj)
end)