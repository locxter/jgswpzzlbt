// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the mount
module cAxisMotorMount()
{
    union()
    {   
        // Left support
        hull()
        {
            cube([5, 41.75, 5]);
            translate([0, 36.75, 5])
            {
                cube([5, 5, 36.75]);
            }
        }
        // Motor mount
        translate([5, 0, 0])
        {
            difference()
            {
                // Base shape
                cube([29, 41.75, 5]);
                // Hole cutouts
                translate([3, 6.5, -1])
                {
                    cylinder(7, 1.5, 1.5);
                }
                translate([26, 6.5, -1])
                {
                    cylinder(7, 1.5, 1.5);
                }
                translate([3, 29.5, -1])
                {
                    cylinder(7, 1.5, 1.5);
                }
                translate([26, 29.5, -1])
                {
                    cylinder(7, 1.5, 1.5);
                }
                translate([14.5, 18, -1])
                {
                    cylinder(7, 11.25, 11.25);
                }
            }
        }
        // Mounting plate
        translate([5, 36.75, 5])
        {
            difference()
            {
                // Base Shape
                cube([29, 5, 36.75]);
                // Left slot
                hull()
                {
                    translate([4.25, -1, 7.75])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                    translate([4.25, -1, 29])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                }
                // Right slot
                hull()
                {
                    translate([24.75, -1, 7.75])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                    translate([24.75, -1, 29])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                }
            }
        }
        // Right support
        translate([34, 0, 0])
        {
            hull()
            {
                cube([5, 41.75, 5]);
                translate([0, 36.75, 5])
                {
                    cube([5, 5, 36.75]);
                }
            }
        }
    }
}

// Using the module
cAxisMotorMount();
