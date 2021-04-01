-- lua/lua.lua

premake.modules.deploy = {}
local m = premake.modules.deploy

local p = premake

newaction {
	trigger = "deploy",
	description = "Deploy Ivy sdk",

	onStart = function()
		print("Starting Ivy deploy")
	end,

	onWorkspace = function(wks)
		--printf("Generating Lua for workspace '%s'", wks.basedir)
		local wksDir = wks.basedir:gsub("/", "\\")
		local ivyDir = wks.projects["Ivy"].location:gsub("/", "\\")
		local ivyTargetDir = wks.projects["Ivy"].configs["Release"].targetdir:gsub("/", "\\") .. "\\" .. wks.projects["Ivy"].name .. ".lib"
		
		local sandboxDir = wks.projects["Sandbox"].location:gsub("/", "\\")
		local sandboxTargetDir = wks.projects["Sandbox"].configs["Release"].targetdir:gsub("/", "\\")
		print(sandboxDir)
		print(sandboxTargetDir)

		local cpyExample = "xcopy \"" .. sandboxDir .. "\\*\" \"" .. wksDir .. "\\sdk\\example\\ \" /y /e /i /f"
		local cpyExampleExe = "xcopy \"" .. sandboxTargetDir .. "\\*.exe\" \"" .. wksDir .. "\\sdk\\example\\ \" /y /e /i /f"
		local cpyExampleDll = "xcopy \"" .. sandboxTargetDir .. "\\*.dll\" \"" .. wksDir .. "\\sdk\\example\\ \" /y /e /i /f"
		os.execute(cpyExample)
		os.execute(cpyExampleExe)
		os.execute(cpyExampleDll)


		local cpyInclude = "xcopy \"" .. ivyDir .. "\\source\\*.h\" \"" .. wksDir .. "\\sdk\\include\\ \" /y /e /i /f"
		local cpyIvyLib = "xcopy \"" .. ivyTargetDir .."\" \"" .. wksDir .. "\\sdk\\lib\\ \" /y /e /i /f"
		local cpyDeps = "xcopy \"" .. ivyDir .. "\\dependencies\\*.h\" \"" .. wksDir .. "\\sdk\\dependencies\\ \" /y /e /i /f"
		local cpyDepsHpp = "xcopy \"" .. ivyDir .. "\\dependencies\\*.hpp\" \"" .. wksDir .. "\\sdk\\dependencies\\ \" /y /e /i /f"

		local cpyDepLibs = "for /r \"" .. ivyDir .. "\\dependencies\" %f in (*.lib) do @copy \"%f\" \"" .. wksDir .. "\\sdk\\lib\""
		local cpyDepDll = "for /r \"" .. ivyDir .. "\\dependencies\" %f in (*.dll) do @copy \"%f\" \"" .. wksDir .. "\\sdk\\bin\""

		os.execute(cpyInclude)
		os.execute(cpyIvyLib)
		os.execute(cpyDeps)
		os.execute(cpyDepsHpp)
		os.execute(cpyDepLibs)
		os.execute(cpyDepDll)
		os.execute("ROBOCOPY sdk sdk /S /MOVE") -- deletes empty folders


		--printf( wks.projects["Ivy"].configs["Release"].linktarget)
		--for k,v in pairs(wks.projects["Ivy"].configs["Release"].files) do
		--	printf(k)
		--end
			
			
	end,

	onProject = function(prj)

	end,

	execute = function()
		print("Executing Lua action")
	end,

	onEnd = function()
		print("Deploy complete. Generated all files in sdk/")
	end
}

return m


-- key: global
-- key: configmap
-- key: platforms
-- key: terms
-- key: tags
-- key: class
-- key: kind
-- key: location
-- key: solution
-- key: project
-- key: blocks
-- key: script
-- key: _cfgset
-- key: xcodesystemcapabilities
-- key: compiled
-- key: _
-- key: environ
-- key: external
-- key: basedir
-- key: architecture
-- key: uuid
-- key: rules
-- key: _basedir
-- key: workspace
-- key: language
-- key: system
-- key: parent
-- key: filename
-- key: configurations
-- key: current
-- key: nugetsource
-- key: buildcustomizations
-- key: nuget
-- key: _isBaked
-- key: name
-- key: configs
-- key: vpaths
-- key: _cfglist
-- key: group