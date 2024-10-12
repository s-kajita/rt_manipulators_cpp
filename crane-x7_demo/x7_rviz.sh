#!/bin/bash
read -p "Turn on switch, then hit key"
roslaunch crane_x7_bringup demo.launch fake_execution:=false
