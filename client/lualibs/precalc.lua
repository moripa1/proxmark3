--[[
    This is an experimental lib.
--]]
local utils = require('utils')

-- LOOKUP Tables
local perm = {}
perm [1]= { 0x0, 0x1, 0x3, 0x2, 0x7, 0x6, 0x4, 0x5, 0xF, 0xE, 0xC, 0xD, 0x8, 0x9, 0xB, 0xA }
perm [2]= { 0x1, 0x0, 0x2, 0x3, 0x6, 0x7, 0x5, 0x4, 0xE, 0xF, 0xD, 0xC, 0x9, 0x8, 0xA, 0xB }
perm [3]= { 0x2, 0x3, 0x1, 0x0, 0x5, 0x4, 0x6, 0x7, 0xD, 0xC, 0xE, 0xF, 0xA, 0xB, 0x9, 0x8 }
perm [4]= { 0x3, 0x2, 0x0, 0x1, 0x4, 0x5, 0x7, 0x6, 0xC, 0xD, 0xF, 0xE, 0xB, 0xA, 0x8, 0x9 }
perm [5]= { 0x4, 0x5, 0x7, 0x6, 0x3, 0x2, 0x0, 0x1, 0xB, 0xA, 0x8, 0x9, 0xC, 0xD, 0xF, 0xE }
perm [6]= { 0x5, 0x4, 0x6, 0x7, 0x2, 0x3, 0x1, 0x0, 0xA, 0xB, 0x9, 0x8, 0xD, 0xC, 0xE, 0xF }
perm [7]= { 0x6, 0x7, 0x5, 0x4, 0x1, 0x0, 0x2, 0x3, 0x9, 0x8, 0xA, 0xB, 0xE, 0xF, 0xD, 0xC }
perm [8]= { 0x7, 0x6, 0x4, 0x5, 0x0, 0x1, 0x3, 0x2, 0x8, 0x9, 0xB, 0xA, 0xF, 0xE, 0xC, 0xD }
perm [9]= { 0x8, 0x9, 0xB, 0xA, 0xF, 0xE, 0xC, 0xD, 0x7, 0x6, 0x4, 0x5, 0x0, 0x1, 0x3, 0x2 }
perm [10]= { 0x9, 0x8, 0xA, 0xB, 0xE, 0xF, 0xD, 0xC, 0x6, 0x7, 0x5, 0x4, 0x1, 0x0, 0x2, 0x3 }
perm [11]= { 0xA, 0xB, 0x9, 0x8, 0xD, 0xC, 0xE, 0xF, 0x5, 0x4, 0x6, 0x7, 0x2, 0x3, 0x1, 0x0 }
perm [12]= { 0xB, 0xA, 0x8, 0x9, 0xC, 0xD, 0xF, 0xE, 0x4, 0x5, 0x7, 0x6, 0x3, 0x2, 0x0, 0x1 }
perm [13]= { 0xC, 0xD, 0xF, 0xE, 0xB, 0xA, 0x8, 0x9, 0x3, 0x2, 0x0, 0x1, 0x4, 0x5, 0x7, 0x6 }
perm [14]= { 0xD, 0xC, 0xE, 0xF, 0xA, 0xB, 0x9, 0x8, 0x2, 0x3, 0x1, 0x0, 0x5, 0x4, 0x6, 0x7 }
perm [15]= { 0xE, 0xF, 0xD, 0xC, 0x9, 0x8, 0xA, 0xB, 0x1, 0x0, 0x2, 0x3, 0x6, 0x7, 0x5, 0x4 }
perm [16]= { 0xF, 0xE, 0xC, 0xD, 0x8, 0x9, 0xB, 0xA, 0x0, 0x1, 0x3, 0x2, 0x7, 0x6, 0x4, 0x5 }

local shifts = {}
shifts[1]= { 0x4, 0x5, 0x7, 0x6, 0x3, 0x2, 0x0, 0x1, 0xB, 0xA, 0x8, 0x9, 0xC, 0xD, 0xF, 0xE }
shifts[2]= { 0x4, 0xB, 0xB, 0x4, 0xB, 0x4, 0x4, 0xB, 0xA, 0x5, 0x5, 0xA, 0x5, 0xA, 0xA, 0x5 }
shifts[3]= { 0xB, 0x6, 0x0, 0xD, 0xD, 0x0, 0x6, 0xB, 0x6, 0xB, 0xD, 0x0, 0x0, 0xD, 0xB, 0x6 }
shifts[4]= { 0xE, 0x5, 0x9, 0x2, 0x0, 0xB, 0x7, 0xC, 0x3, 0x8, 0x4, 0xF, 0xD, 0x6, 0xA, 0x1 }
shifts[5]= { 0x4, 0xE, 0x1, 0xB, 0xF, 0x5, 0xA, 0x0, 0x3, 0x9, 0x6, 0xC, 0x8, 0x2, 0xD, 0x7 }
shifts[6]= { 0xA, 0x4, 0x7, 0x9, 0x0, 0xE, 0xD, 0x3, 0xE, 0x0, 0x3, 0xD, 0x4, 0xA, 0x9, 0x7 }
shifts[7]= { 0xE, 0x6, 0xE, 0x6, 0xF, 0x7, 0xF, 0x7, 0xD, 0x5, 0xD, 0x5, 0xC, 0x4, 0xC, 0x4 }
shifts[8]= { 0x7, 0x1, 0xB, 0xD, 0xE, 0x8, 0x2, 0x4, 0x4, 0x2, 0x8, 0xE, 0xD, 0xB, 0x1, 0x7 }
shifts[9]= { 0xD, 0xB, 0x0, 0x6, 0x6, 0x0, 0xB, 0xD, 0xA, 0xC, 0x7, 0x1, 0x1, 0x7, 0xC, 0xA }
shifts[10]= { 0xe, 0x1, 0x1, 0xe, 0x1, 0xe, 0xe, 0x1, 0x1, 0xe, 0xe, 0x1, 0xe, 0x1, 0x1, 0xe }

local function ApplyPermutationAndShifts( pos, value, nibble)
    local shiftbytes = shifts[pos]
    local shiftElem = shiftbytes[nibble+1] --one indexed
    local shiftOne = shiftbytes[1]
    local rs = bit32.bxor(value, shiftOne, shiftElem)
    return rs
end

local function GetOne( uid, block )

    if uid == nil then return nil, 'empty uid string' end
    if #uid == 0 then return nil, 'empty uid string' end
    if #uid ~= 8 then return nil, 'uid wrong length. Should be 4 hex bytes' end
    if type(block) ~= 'number' then return nil, 'block is not number' end
    if  block > 16 or block < 0 then return nil, 'block is out-of-range' end

    local s = ('%s%02X'):format(uid,block)
    local nibble1 = tonumber(s:sub(1,1),16) + 1

    local permuted = ''
    for i = 1, #s do
        local el_row = shifts[i]
        local el_value = el_row[nibble1]
        j = 1
        while j <= i do
            if  i-j > 0 then
                local nibble = tonumber(s:sub(j+1,j+1),16)
                el_value = ApplyPermutationAndShifts(i-j, el_value, nibble)
            end
            j = j+1
        end
        permuted =('%s%X'):format(permuted,el_value)
    end

    permuted = 'C2'..permuted
    local crc64numStr = utils.Crc64(permuted)
    local keybytes = utils.ConvertAsciiToBytes(crc64numStr, true)
    local key = utils.ConvertBytesToHex(keybytes)
    return key:sub(1,12)
end

local PreCalc =
{
    GetAll = function(id)
        if id == nil then return nil, 'empty string' end
        if #id == 0 then return nil, 'empty string' end
        if #id ~= 8 then return nil, 'wrong length. Should be 4 hex bytes' end

        local list = '4b0b20107ccb'
        for i = 1,15 do
            local key, err  = GetOne(id,i)
            if not key then return oops(err) end
            list = list..key
        end
        return list
    end,
}
return PreCalc