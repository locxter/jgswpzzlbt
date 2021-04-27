#!/bin/bash
echo "Started generating the assembly image."
openscad -o assembly.png --render --imgsize=3840,2160 assembly.scad
mogrify -resize 1920x1080 assembly.png
echo "Generated the assembly image successfully."
