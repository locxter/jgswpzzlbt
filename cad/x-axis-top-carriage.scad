// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the carriage
module xAxisTopCarriage()
{
    union()
    {
        // Base plate
        difference()
        {
            // Base shape
            hull()
            {
                translate([32.5, 0, 0])
                {
                    cube([15.5, 25.5, 5]);
                }
                translate([0, 61, 0])
                {
                    cube([15.5, 15.5, 5]);
                }
                translate([65, 61, 0])
                {
                    cube([15.5, 15.5, 5]);
                }
                translate([0, 105, 0])
                {
                    cube([15.5, 15.5, 5]);
                }
                translate([65, 105, 0])
                {
                    cube([15.5, 15.5, 5]);
                }
                translate([27.5, 121.5, 0])
                {
                    cube([25.5, 5, 5]);
                }
            }
            // Front wheel slot
            hull()
            {
                translate([40.25, 7.75, -1])
                {
                    cylinder(7, 2.75, 2.75);
                }
                translate([40.25, 17.75, -1])
                {
                    cylinder(7, 2.75, 2.75);
                }
            }
            // Belt screw holes
            translate([7.75, 68.75, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
            translate([72.75, 78.75, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
            // Rear wheel holes
            translate([7.75, 112.75, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
            translate([72.75, 112.75, -1])
            {
                cylinder(7, 2.75, 2.75);
            }
        }
        // Cable chain mount
        translate([27.5, 121.5, 5])
        {
            difference()
            {
                // Base shape
                cube([25.5, 5, 37]);
                // Mounting holes
                translate([10.5, -1, 12.5])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(7, 1.75, 1.75);
                    }
                }
                translate([15, -1, 19.5])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(7, 1.75, 1.75);
                    }
                }
            }
        }
    }
}

// Using the module
xAxisTopCarriage();
