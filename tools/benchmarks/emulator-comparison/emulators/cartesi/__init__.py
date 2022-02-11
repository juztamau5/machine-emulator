# Copyright 2022 Cartesi Pte. Ltd.
#
# This file is part of the machine-emulator. The machine-emulator is free
# software: you can redistribute it and/or modify it under the terms of the GNU
# Lesser General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# The machine-emulator is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the machine-emulator. If not, see http://www.gnu.org/licenses/.

class Emulator:
    def __init__(self, config, bench_cmd):
        self.config = config
        self.bench_cmd = bench_cmd

    def build_command(self):
      return "{}/bin/cartesi-machine.lua --ram-image={} --rom-image={} --flash-drive=label:root,filename:{} -- '{}'".format(
              self.config['path'], self.config['kernel'], self.config['rom'], self.config['drive'], self.bench_cmd)

    def build_env(self):
        return {"LUA_CPATH": "{0}/lib/?.so;{0}/lib/lua/5.3/?.so;{0}/lib/lua/5.3/cartesi/?.so;".format(self.config['path']),
                "LUA_PATH": "{0}/bin/?.lua;{0}/share/lua/5.3/?.lua".format(self.config['path']),
                "LD_LIBRARY_PATH": "{}/lib/?.so".format(self.config['path'])}
