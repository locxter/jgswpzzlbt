// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the spacer
module xAxisPulleySpacer ()
{
    difference()
    {
        // Base shape
        hull()
        {
            cylinder(2, 7.75, 7.75);
            translate([0, 0, 2])
            {
                cylinder(2, 3.75, 3.75);
            }
        }
        // Screw hole
        translate([0, 0, -1])
        {
            cylinder(6, 2.75, 2.75);
        }
    }
}

// Using the module
xAxisPulleySpacer();
