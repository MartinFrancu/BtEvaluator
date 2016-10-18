function widget:GetInfo()
  return {
    name      = "BtEvaluator loader",
    desc      = "BtEvaluator loader and message test to this AI.",
    author    = "JakubStasta",
    date      = "Sep 20, 2016",
    license   = "BY-NC-SA",
    layer     = 0,
    enabled   = true, --  loaded by default?
	version   = version,
  }
end

local Chili
local Screen0

local testbutton
local msgCommandButton
local moveCommandButton

function listenerOnTestButtonClick(self)
	Spring.Echo("Test message sent from widget to C++ Skirmish AI. ")
	Spring.SendSkirmishAIMessage(Spring.GetLocalPlayerID(), "Test message from widget - what is written here? Heh?. ")
	return true
end
function listenerOnMessageButtonClick (self)
	Spring.Echo ("Message command sent from widget.")
	Spring.SendSkirmishAIMessage (Spring.GetLocalPlayerID (), "Msg MessageCommand executed")
	return true
end
function listenerOnMoveButtonClick (self)
	Spring.Echo ("Move command sent from widget.")
	Spring.SendSkirmishAIMessage (Spring.GetLocalPlayerID (), "Move")
	return true
end



function widget:Initialize()	
  if (not WG.Chili) then
    -- don't run if we can't find Chili
    widgetHandler:RemoveWidget()
    return
  end
 
  -- Get ready to use Chili
  Chili = WG.Chili
  Screen0 = Chili.Screen0
	
	testbutton = Chili.Button:New{
		parent = Screen0,
		caption = "Send Message to AI",
		x = '80%',
		y = '90%',
		OnClick = { listenerOnTestButtonClick },
		width = 150,
		height = 60,
	}

	msgCommandButton = Chili.Button:New{
		parent = Screen0,
		caption = "Send message command",
		x = '70%',
		y = '90%',
		OnClick = { listenerOnMessageButtonClick },
		width = 150,
		height = 60,
	}

	moveCommandButton = Chili.Button:New {
		parent = Screen0,
		caption = "Move selected",
		x = '60%',
		y = '90%',
		OnClick = { listenerOnMoveButtonClick },
		width = 150,
		height = 60,
	}

	-- Add control of my units also to our BtEvaluator -> Cpp AI
	Spring.SendCommands("AIControl "..Spring.GetLocalPlayerID().." BtEvaluator")
	-- Spring.Echo("GetLocalPlayerID(): "..Spring.GetLocalPlayerID())
end

function widget:RecvSkirmishAIMessage(aiTeam, aiMessage)
	Spring.Echo("Message from AI received: "..aiMessage)
end