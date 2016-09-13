#!/usr/bin/bash

source /opt/Xilinx/14.7/ISE_DS/settings64.sh

impact -batch impact.cmd || exit 1
time clujtag -p /dev/ttyACM1 -x default.xsvf
