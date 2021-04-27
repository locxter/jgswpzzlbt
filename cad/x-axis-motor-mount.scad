// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;    

// Module for creating the motor mount
module xAxisMotorMount()
{
    union()
    {
        // Base plate
        difference()
        {
            // Base shape
            hull()
            {
                cube([90.5, 40.25, 5]);
                translate([23, 65.25, 0])
                {
                    cube([44.5, 44.5, 5]);
                }
                translate([90.5, 0, 0])
                {
                    
                    cube([70.75, 30.25, 5]);
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
            // Motor mounting holes
            translate([29.75, 72, -1])
            {
                cylinder(7, 1.75, 1.75);
            }
            translate([60.75, 72, -1])
            {
                cylinder(7, 1.75, 1.75);
            }
            translate([29.75, 103, -1])
            {
                cylinder(7, 1.75, 1.75);
            }
            translate([60.75, 103, -1])
            {
                cylinder(7, 1.75, 1.75);
            }
            translate([45.25, 87.5, -1])
            {
             cylinder(7, 11.25, 11.25);
            }
        }
        // Cable chain mount
        translate([130.75, 0, 0])
        {
            // Left support
            hull()
            {
                cube([30.5, 5, 5]);
                translate([25.5, 0, 5])
                {
                    cube([5, 5, 25.5]);
                }
            }
            // Mounting plate
            translate([25.5, 5, 5])
            {
                difference()
                {
                    // Base shape
                    cube([5, 20.25, 25.5]);
                    // Mounting holes
                    translate([-1, 12.375, 11.75])
                    {
                        rotate([0, 90, 0])
                        {
                            cylinder(7, 1.75, 1.75);
                        }
                    }
                    translate([-1, 7.875, 18.75])
                    {
                        rotate([0, 90, 0])
                        {
                            cylinder(7, 1.75, 1.75);
                        }
                    }
                }
            }
            // Right support
            translate([0, 25.25, 0])
            {
                hull()
                {
                    cube([30.5, 5, 5]);
                    translate([25.5, 0, 5])
                    {
                        cube([5, 5, 25.5]);
                    }
                }
            }
        }
    }
}

// Using the module
xAxisMotorMount();
