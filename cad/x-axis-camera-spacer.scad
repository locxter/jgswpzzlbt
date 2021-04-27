// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the spacer
module xAxisCameraSpacer()
{
    difference()
    {
        // Base shape
        cylinder(12, 2.25, 2.25);
        // Hole
        translate([0, 0, -1])
        {
            cylinder(14, 1.25, 1.25);
        }
    }
}

// Using the module
xAxisCameraSpacer();
