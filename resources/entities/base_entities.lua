testEnt = {
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
		maxSpeed = 200,
		friction = 3,
		mass = 1
	},
	collisionBody = {
	},
    size = {
        x = 32,
        y = 32,
    },
	maxHeight = 300,
	objectTraveledAlt = 0,
	oldAlt = 0,
	objectJump = false,
	objectMaxHeight = false,
	update = function(object)
		acceleration = 40
		if (object:getNormalForce().y <= 0) then
			acceleration = 15
		end
		
		if (isInputPressed(inputs.LShift)) then
			acceleration = acceleration + acceleration * 0.2
		end

		if (isInputPressed(inputs.S)) then
			object:applyForce(0, acceleration)
		elseif (isInputPressed(inputs.W)) then
			object:applyForce(0, -acceleration)
		end
	
		if (isInputPressed(inputs.D)) then
			object:applyForce(acceleration, 0)
		elseif (isInputPressed(inputs.A)) then
			object:applyForce(-acceleration, 0)
		end

		if (isInputPressed(inputs.Space) and not testEnt["objectMaxHeight"]) then
			baseForce = 50
			-- force = ((currentH - maxH) * initF) / -maxH
			appliedForce = ((testEnt["objectTraveledAlt"] - testEnt["maxHeight"]) * baseForce) / -testEnt["maxHeight"]
			object:applyForce(0, -appliedForce)
			print(appliedForce)
		end
		
		if ((not isInputPressed(inputs.Space) and testEnt["objectJump"]) or (testEnt["objectJump"] and object:getForce().y > 0)) then
			testEnt["objectMaxHeight"] = true
		end
		
		if (testEnt["objectJump"]) then
			travelAlt = testEnt["objectTraveledAlt"]
			if (testEnt["oldAlt"] - object:getPosition().y >= 0) then
				testEnt["objectTraveledAlt"] = travelAlt + testEnt["oldAlt"] - object:getPosition().y
				testEnt["oldAlt"] = object:getPosition().y
			end
		end
		
		if (object:getNormalForce().y > 0) then
			testEnt["objectJump"] = false
			testEnt["objectTraveledAlt"] = 0
		end
		
		if (isInputPressed(inputs.Space) and object:getNormalForce().y > 0) then
			testEnt["objectJump"] = true
			testEnt["oldAlt"] = object:getPosition().y
			testEnt["objectMaxHeight"] = false
			object:applyForce(0, -5000)
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
		end
	},
    size = {
        x = 16,
        y = 16,
    }
}
