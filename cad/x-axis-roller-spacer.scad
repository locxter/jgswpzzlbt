// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the spacer
module xAxisRollerSpacer ()
{
    difference()
    {
        // Base shape
        hull()
        {
            cylinder(5.5, 7.75, 7.75);
            translate([0, 0, 5.5])
            {
                cylinder(5.5, 3.75, 3.75);
            }
        }
        // Screw hole
        translate([0, 0, -1])
        {
            cylinder(13, 2.75, 2.75);
        }
    }
}

// Using the module
xAxisRollerSpacer();
