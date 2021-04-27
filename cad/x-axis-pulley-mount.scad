// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the pulley mount
module xAxisPulleyMount()
{
    difference()
    {
        // Base shape
        hull()
        {
            cube([90.5, 40.25, 5]);
            translate([41, 40.25, 0])
            {
                cube([18.5, 35.5, 5]);
            }
        }
        // Mounting holes
        translate([15.25, 15.125, -1])
        {
            cylinder(7, 2.75, 2.75);
        }
        translate([75.25, 15.125, -1])
        {
            cylinder(7, 2.75, 2.75);
        }
        // Pulley slot
        hull()
        {
            translate([50.25, 48, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
            translate([50.25, 68, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
        }
    }
}

// Using the module
xAxisPulleyMount();
