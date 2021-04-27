// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Module for creating the carriage
module xAxisBottomCarriage()
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
            // Camera mounting holes
            translate([26.25, 43.75, -1])
            {
                cylinder(7, 1.25, 1.25);
            }
            translate([26.25, 71.75, -1])
            {
                cylinder(7, 1.25, 1.25);
            }
            translate([54.25, 71.75, -1])
            {
                cylinder(7, 1.25, 1.25);
            }
            translate([54.25, 43.75, -1])
            {
                cylinder(7, 1.25, 1.25);
            }
            // Endstop mounting holes
            translate([77.5, 64.5, -1])
            {
                cylinder(7, 1.75, 1.75);
            }
            translate([77.5, 83.5, -1])
            {
                cylinder(7, 1.75, 1.75);
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
            // Cable chain mount cutout
            translate([27.25, 121.25, -1])
            {
                cube([26, 6.25, 7]);
            }
        }
        // Motor mount
        translate([15.5, 78.5, 0])
        {
            union()
            {
                // Left support
                hull()
                {
                    cube([5, 42, 5]);
                    translate([0, 37, 5])
                    {
                        cube([5, 5, 37]);
                    }
                }
                // Mounting plate
                translate([5, 37, 5])
                {
                    difference()
                    {
                        // Base shape
                        cube([39.5, 5, 37]);
                        // Left slot
                        hull()
                        {
                            translate([9.5, -1, 8])
                            {
                                rotate([-90, 0, 0])
                                {
                                    cylinder(7, 2.75, 2.75);
                                }
                            }
                            translate([9.5, -1, 29.25])
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
                            translate([30, -1, 8])
                            {
                                rotate([-90, 0, 0])
                                {
                                    cylinder(7, 2.75, 2.75);
                                }
                            }
                            translate([30, -1, 29.25])
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
                translate([44.5, 0, 0])
                {
                    hull()
                    {
                        cube([4, 42, 5]);
                        translate([0, 37, 5])
                        {
                            cube([4, 5, 37]);
                        }
                    }
                }
            }
        }
    }
}

// Using the module
xAxisBottomCarriage();
