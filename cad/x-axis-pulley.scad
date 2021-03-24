// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisPulley()
{
    difference()
    {
        union()
        {
            cylinder(1.75, 13, 13);
            translate([0, 0, 1.75])
            {
                cylinder(6.5, 10, 10);
            }
            translate([0, 0, 8.25])
            {
                cylinder(1.75, 13, 13);
            }
        }
        translate([0, 0, -1])
        {
            cylinder(12, 8, 8);
        }
    }
}

xAxisPulley();

