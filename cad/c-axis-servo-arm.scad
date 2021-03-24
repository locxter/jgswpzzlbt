// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module cAxisServoArm()
{
    difference()
    {
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
        translate([-5.5, -6.125, -6.25])
        {
            cube([36, 12.25, 6.25]);
        }
        translate([0, 0, 2.25])
        {
            cylinder(4, 2.5, 2.5);
        }
        translate([7.75, 0, 0])
        {
            sphere(3.25);
        }
        translate([7.75, 0, 0])
        {
            rotate([0, 90, 0])
            {
                cylinder(22.75, 3.25, 3.25);
            }
        }
    }
}

cAxisServoArm();
