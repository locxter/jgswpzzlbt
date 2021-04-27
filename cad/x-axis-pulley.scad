// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for Creating the pulley
module xAxisPulley()
{
    difference()
    {
        // Base shape
        union()
        {
            cylinder(1.875, 13, 13);
            translate([0, 0, 1.875])
            {
                cylinder(6.25, 10, 10);
            }
            translate([0, 0, 8.125])
            {
                cylinder(1.875, 13, 13);
            }
        }
        // Bearing cutout
        translate([0, 0, -1])
        {
            cylinder(12, 8, 8);
        }
    }
}

// Using the module
xAxisPulley();

