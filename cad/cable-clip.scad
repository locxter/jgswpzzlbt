// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module cableClip()
{
    difference()
    {
        union()
        {
            cube([11, 1, 5]);
            translate([3, 1, 0])
            {
                cube([5, 4, 5]);
            }
        }
        translate([4, -1, -1])
        {
            cube([3, 5, 7]);
        }
    }
}

cableClip();
