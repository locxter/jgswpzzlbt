// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the clip
module cableClip()
{
    difference()
    {
        // Base shape
        union()
        {
            cube([11, 1, 5]);
            translate([3, 1, 0])
            {
                cube([5, 4, 5]);
            }
        }
        // Cable cutout
        translate([4, -1, -1])
        {
            cube([3, 5, 7]);
        }
    }
}

// Using the module
cableClip();
