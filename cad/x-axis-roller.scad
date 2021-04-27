// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the roller
module xAxisRoller()
{
    difference()
    {
        // Base shape
        union()
        {
            cylinder(2.625, 10, 10);
            translate([0, 0, 2.625])
            {
                cylinder(4.75, 13, 13);
            }
            translate([0, 0, 7.375])
            {
                cylinder(2.625, 10, 10);
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
xAxisRoller();
