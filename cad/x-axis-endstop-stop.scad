// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisEndstopStop()
{
    difference()
    {
        hull()
        {
            cube([35.5, 15.5, 10]);
            translate([0, 10.5, 10])
            {
                cube([35.5, 5, 5]);
            }
        }
        translate([7.75, 7.75, -1])
        {
            cylinder(17, 2.75, 2.75);
        }
        translate([7.75, 7.75, 10])
        {
            cylinder(6, 4.5, 4.5);
        }
        translate([27.75, 7.75, -1])
        {
            cylinder(17, 2.75, 2.75);
        }
        translate([27.75, 7.75, 10])
        {
            cylinder(6, 4.5, 4.5);
        }
    }
}

xAxisEndstopStop();
