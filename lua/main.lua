
--[[region disable global variables]]
local __g = _G

-- export global variable
--[[
    attention here!!!!
    st is opened by c++ code, so st is a table here.
]]
sg = {}
sg.exports = {}
sg.exports.UIScaleRatio=0
sg.exports.UIScaleInitRatio =0
sg.exports.screenSize = {w = 0, h = 0}

-- sg.export.bDisabledAI = true

setmetatable(sg.exports, {
    __newindex = function(_, name, value)
        rawset(__g, name, value)
    end,

    __index = function(_, name)
        return rawget(__g, name)
    end
})

-- disable create unexpected global variable
setmetatable(__g, {
    __newindex = function(t, name, value)
        local msg = "USE 'sg.exports.%s = value' INSTEAD OF SET GLOBAL VARIABLE"
        error(string.format(msg, name), 0)
    end
})


function sg.exports.init (index,protoid,bin)
    print("enter sg.exports.init!protoid:",protoid)

end

function sg.exports.update(nFrames)
    print("sg.exports.update",nFrames)
end

function sg.exports.connectToUdpServer(ip, port)
    ak.LuaRobot.connectToUdpServer(sg.exports.robot_index,ip, port)
end

function sg.exports.callUdpRpc(protoId, data)
    ak.LuaRobot.callUdpRpc(sg.exports.robot_index,protoId, data)
end

function sg.exports.onUdpRpcSuccess(type, bin)
    print("enter onUdpRpcSuccess!battleid:",sg.exports.battle_id,"protoid:",type)
end


function sg.exports.onUdpRpcError(type, resposne)
    print("enter onUdpRpcError",inspect(type))
end
