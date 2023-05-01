#!/usr/bin/lua5.3

-- Copyright 2023 Ye Jiaqiang <yejq.jiaqiang@gmail.com>
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

local function bitdump_string(strint)
	local value = tonumber(strint)
	if type(value) ~= "number" or math.type(value) ~= "integer" then
		io.stderr:write(string.format("Error, invalid integer, skipped: %s\n", strint))
		io.stderr:flush()
		return false
	end

	local nbits = 32
	if value > 0xFFFFFFFF or value < -2147483648 then
		nbits = 64
	end

	local bits = {}
	for idx = 1, nbits do
		local mbits = idx - 0x1
		bits[#bits + 0x1] = (value & (0x1 << mbits)) == 0 and "0" or "1"
		if (idx & 0x3) == 0 then bits[#bits + 0x1] = "  " end
	end
	bits[#bits + 0x1] = "  "
	io.stdout:write(string.format("   Value: %s (%#x, %d) => \n", strint, value, value))
	if nbits > 32 then
		io.stdout:write("   63    59    55    51    47    43    39    35 ")
	end
	io.stdout:write("   31    27    23    19    15    11     7     3\n")
	io.stdout:write(string.reverse(table.concat(bits)) .. "\n")
	if nbits > 32 then
		io.stdout:write("       60    56    52    48    44    40    36    32")
	else
		io.stdout:write("   ")
	end
	io.stdout:write("    28    24    20    16    12    8     4     0\n")
	io.stdout:write("--------------------------------------------------\n")
	io.stdout:flush()
	return true
end

local idx = 0
while true do
	idx = idx + 0x1
	local argv = arg[idx]
	if type(argv) ~= "string" then break end
	bitdump_string(argv)
end
os.exit(0)
