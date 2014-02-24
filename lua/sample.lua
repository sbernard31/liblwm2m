local lwm2m = require 'lwm2m'
local socket = require 'socket'

local udp = socket.udp();
udp:setsockname('*', 5683)

local deviceObj = {
  id = 3,
  [0]  = "Open Mobile Alliance",                   -- manufacturer
  [1]  = "Lightweight M2M Client",                 -- model number
  [2]  = "345000123",                              -- serial number
  [3]  = "1.0",                                    -- firmware version
  [13] = {read = function() return os.time() end}, -- current time
}

local sampleObj = {
  id = 3,
  -- READ
  [0]  = "Res0 : Read only",
  [1]  = {read = "Res1 : Read only"},
  [2]  = {read = function (obj) return "Res2: Read only" end},
  
  -- EXECUTE
  [4]  = {execute = function (obj) print ("Res4: execute !") end},

  -- READ/WRITE/EXECUTE
  [13]  = function (obj, mode, value)
    if mode == "read" then
      return "Res13: Read/Write/Execute, mode=" .. mode
    elseif mode == "write" then
      print ("Res13: Read/Write/Execute, value=",value, ", mode=",mode)
    elseif mode == "execute" then
      print ("Res13: Read/Write/Execute, mode=",mode)
    end
  end,

  -- READ/WRITE
  [14]  = {write = function (obj, value) print ("Res14: Write only:",value) end},
  [15] = {
    value = "Res15: defaultvalue",
    read  = function (obj)
      local val = obj[15].value
      print (val); return val;
    end,
    write = function (obj, value)
      print("Res15 modification")
      print("before :", obj[15].value," after:",value)
      obj[15] = value
    end
  }
}

local ll = lwm2m.init("testlualwm2mclient", {deviceObj},
  function(data,host,port) udp:sendto(data,host,port) end)

ll:addserver(123, "127.0.0.1",5684)
ll:register()

repeat
  ll:step()
  local data, ip, port, msg = udp:receivefrom()
  if data then
    ll:handle(data,ip,port)
  end
until false