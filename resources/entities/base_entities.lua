player = {
    sceneGraph = {
        renderType = "renderObject",
        colour = {
            r = 0,
            g = 0,
            b = 255,
            a = 255
        }
    },
	rigidBody = {
		maxSpeed = 600,
		friction = 10,
		mass = 1
	},
	collisionBody = {
	},
    size = {
        x = 32,
        y = 32,
    },
	events = {
		coin_collected = function()
			player["maxHeight"] = player["maxHeight"] * (5 / 8)
		end
	},
	onAdd = function(object)
		player["maxHeight"] = 300
		player["objectTraveledAlt"] = 0
		player["oldAlt"] = 0
		player["objectJump"] = false
		player["hasJumped"] = false
		player["objectMaxHeight"] = false
		player["pressedR"] = false
	end,
	fixedUpdate = function(object, deltaTime)
		acceleration = 1070
		if (object:getNormalForce().y <= 0) then
			acceleration = 1015
		end
		
		if (isInputPressed(inputs.Command)) then
			acceleration = acceleration + acceleration * 0.2
		end

		if (isInputPressed(inputs.Right)) then
			object:applyForce(acceleration, 0)
		elseif (isInputPressed(inputs.Left)) then
			object:applyForce(-acceleration, 0)
		end

		if (isInputPressed(inputs.Period)) then
			baseForce = 550
			-- force = ((currentH - maxH) * initF) / -maxH
			appliedForce = ((player["objectTraveledAlt"] - player["maxHeight"]) * baseForce) / -player["maxHeight"]
			object:applyForce(0, -appliedForce)
		end

		if (isInputPressed(inputs.Period) and object:getNormalForce().y > 0 and not player["hasJumped"] and not player["objectJump"]) then
			player["objectJump"] = true
			player["oldAlt"] = object:getPosition().y
			player["objectMaxHeight"] = false
			player["hasJumped"] = true
			object:applyForce(0, -25000)
		end
	end,
	update = function(object)
		if ((not isInputPressed(inputs.Period) and player["objectJump"]) or 
			(player["objectJump"] and object:getForce().y > 0) or 
			(player["objectTraveledAlt"] - player["maxHeight"] > 0)) then
			player["objectMaxHeight"] = true
		end
	
		if (player["objectJump"]) then
			travelAlt = player["objectTraveledAlt"]
			if (player["oldAlt"] - object:getPosition().y >= 0) then
				player["objectTraveledAlt"] = travelAlt + player["oldAlt"] - object:getPosition().y
				player["oldAlt"] = object:getPosition().y
			end
		end
	
		if (object:getNormalForce().y > 0 and not player["hasJumped"]) then
			player["objectJump"] = false
			player["objectTraveledAlt"] = 0
		end
	
		if (not isInputPressed(inputs.Period) and player["hasJumped"]) then
			player["hasJumped"] = false
			object:applyForce(0, 7000)
		end
			
		if (isInputPressed(inputs.R) and not player["pressedR"]) then
			player["pressedR"] = true
		elseif (not isInputPressed(inputs.R) and player["pressedR"]) then
			sendEvent("revert_position", object)
			player["pressedR"] = false
			print("abc")
		end
			
		setCameraPosition(object:getPosition().x, object:getPosition().y)
	end
}

coin = {
	sceneGraph = {
        renderType = "renderObject",
        colour = {
            r = 255,
            g = 255,
            b = 255,
            a = 255
        }
    },
	collisionBody = {
		event_on_collision = "coin_collected",
		solid = false,
		on_collision = function(object, data)
			object:destroy()
		end,
		size = {
			x = 20,
			y = 20
		}
	},
    size = {
        x = 16,
        y = 16,
    }
}

world_exit = {
	sceneGraph = {
        renderType = "renderObject",
        colour = {
            r = 0,
            g = 255,
            b = 255,
            a = 255
        }
    },
	collisionBody = {
		event_on_collision = "hit_exit",
		solid = false
	},
    size = {
        x = 30,
        y = 60,
    }
}
