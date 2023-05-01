#!/usr/bin/perl

# Copyright 2023 Ye Jiaqiang <yejq.jiaqiang@gmail.com>
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

use strict;
use warnings;

my @args = @ARGV;
unshift(@args, '--color=auto');
unshift(@args, '/usr/bin/ls.exe');
exit(system(@args));
