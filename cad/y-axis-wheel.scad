// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the wheel
module yAxisWheel()
{
    difference()
    {
        // Base shape
        union()
        {
            cylinder(3, 35.5, 35.5);
            translate([0, 0, 3])
            {
                cylinder(6, 35, 35);
            }
            translate([0, 0, 9])
            {
                cylinder(3, 35.5, 35.5);
            }
            translate([0, 0, 12])
            {
                cylinder(6, 35, 35);
            }
            translate([0, 0, 18])
            {
                cylinder(3, 35.5, 35.5);
            }
            translate([0, 0, 21])
            {
                cylinder(6, 35, 35);
            }
            translate([0, 0, 27])
            {
                cylinder(3, 35.5, 35.5);
            }
        }
        // Motor shaft cutout
        translate([0, 0, 15.75])
        {
            cylinder(15.25, 2.75, 2.75);
        }
        // Screw hole
        translate([0, 0, 22.75])
        {
            rotate([0, 90, 0])
            {
                cylinder(36.5, 1.75, 1.75);
            }
        }
        // Screw head cutout
        translate([8.75, 0, 22.75])
        {
            rotate([0, 90, 0])
            {
                cylinder(26.75, 3, 3);
            }
        }
        // Nut cutout
        translate([4.25, -3, 19.25])
        {
            cube([3, 6, 11.75]);
        }
    }
}

// Using the module
yAxisWheel();
