// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the carriage
module yAxisCarriage()
{
    union()
    {
        // Base plate
        difference()
        {
            // Base Shape
            hull()
            {
                cube([47, 47, 10]);
                translate([123, 0, 0])
                {
                    cube([47, 47, 10]);
                }
                translate([39.75, 65, 0])
                {
                    cube([90.5, 110, 10]);
                }
            }
            // Motor mounting holes and cutouts
            translate([8, 8, -1])
            {
                cylinder(12, 1.75, 1.75);
            }
            translate([8, 8, -1])
            {
                cylinder(6, 3, 3);
            }
            translate([39, 8, -1])
            {
                cylinder(12, 1.75, 1.75);
            }
            translate([39, 8, -1])
            {
                cylinder(6, 3, 3);
            }
            translate([8, 39, -1])
            {
                cylinder(12, 1.75, 1.75);
            }
            translate([8, 39, -1])
            {
                cylinder(6, 3, 3);
            }
            translate([39, 39, -1])
            {
                cylinder(12, 1.75, 1.75);
            }
            translate([39, 39, -1])
            {
                cylinder(6, 3, 3);
            }
            translate([23.5, 23.5, -1])
            {
                cylinder(12, 11.25, 11.25);
            }
            // Bearing hole and cutout
            translate([146.5, 23.5, -1])
            {
                cylinder(12, 5, 5);
            }
            translate([146.5, 23.5, 4.5])
            {
                cylinder(6.5, 8, 8);
            }
        }
        // Extrusion mount
        translate([39.75, 109.5, 0])
        {
            difference()
            {
                // Base shape
                hull()
                {
                    cube([90.5, 30, 10]);
                    translate([0, 30, 0])
                    {
                        cube([90.5, 35.5, 40.25]);
                    }
                }
                // Extrusion cutout
                translate([5, 40, 10])
                {
                    cube([80.5, 20.5, 31.25]);
                }
                // Screw holes
                translate([15.25, -1, 25.125])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(67.5, 2.75, 2.75);
                    }
                }
                translate([75.25, -1, 25.125])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(67.5, 2.75, 2.75);
                    }
                }
                // Screw head cutouts
                translate([15.25, -1, 25.125])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(31, 4.5, 4.5);
                    }
                }
                translate([75.25, -1, 25.125])
                {
                    rotate([-90, 0, 0])
                    {
                        cylinder(31, 4.5, 4.5);
                    }
                }
            }
        }
    }
}

// Using the module
yAxisCarriage();
