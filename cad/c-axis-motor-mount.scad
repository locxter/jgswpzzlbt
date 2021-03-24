// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module cAxisMotorMount()
{
    union()
    {
        difference()
        {
            cube([36, 42, 5]);
            translate([6.5, 6.5, -1])
            {
                cylinder(7, 1.5, 1.5);
            }
            translate([29.5, 6.5, -1])
            {
                cylinder(7, 1.5, 1.5);
            }
            translate([6.5, 29.5, -1])
            {
                cylinder(7, 1.5, 1.5);
            }
            translate([29.5, 29.5, -1])
            {
                cylinder(7, 1.5, 1.5);
            }
            translate([18, 18, -1])
            {
                cylinder(7, 11.25, 11.25);
            }
        }
        translate([0, 32.5, 5])
        {
            difference()
            {
                cube([36, 9.5, 44.5]);
                translate([-1, 0, 0])
                {
                    rotate([45, 0, 0])
                    {
                        cube([38, sqrt(40.5), sqrt(40.5)]);
                    }
                }
                translate([-1, -1, 4.5])
                {
                    cube([38, 5.5, 41]);
                }
                hull()
                {
                    translate([7.75, 3.5, 12.25])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                    translate([7.75, 3.5, 36.75])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                }
                hull()
                {
                    translate([28.25, 3.5, 12.25])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                    translate([28.25, 3.5, 36.75])
                    {
                        rotate([-90, 0, 0])
                        {
                            cylinder(7, 2.75, 2.75);
                        }
                    }
                }
            }
        }
    }
}

cAxisMotorMount();
