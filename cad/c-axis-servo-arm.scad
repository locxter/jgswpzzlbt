// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the arm
module cAxisServoArm()
{
    difference()
    {
        // Base shape
        hull()
        {
            cylinder(5.25, 4.5, 4.5);
            translate([4.5, 0, 0])
            {
                rotate([0, 90, 0])
                {
                    cylinder(25, 5.25, 5.25);
                }
            }
        }
        // Box for cutting unnecessary parts away
        translate([-5.5, -6.125, -6.25])
        {
            cube([36, 12.25, 6.25]);
        }
        // Servo shaft cutout
        translate([0, 0, 2.5])
        {
            cylinder(3.75, 2.5, 2.5);
        }
        // End of tube cutout
        translate([7.75, 0, 0])
        {
            sphere(3.25);
        }
        // Main part of tube cutout
        translate([7.75, 0, 0])
        {
            rotate([0, 90, 0])
            {
                cylinder(22.75, 3.25, 3.25);
            }
        }
    }
}

// Using the module
cAxisServoArm();
