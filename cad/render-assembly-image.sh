#!/bin/bash
echo "Started rendering the assembly image."
openscad -o assembly.png --render --imgsize=3840,2160 assembly.scad
mogrify -resize 1920x1080 assembly.png
echo "Rendered the assembly image successfully."
