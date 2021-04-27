// Uncomment before rendering and exporting
//$fa = 0.01;
//$fs = 0.25;

// Importing all needed modules
use <c-axis-motor-mount.scad>
use <c-axis-servo-arm.scad>
use <c-axis-servo-mount.scad>
use <x-axis-bottom-carriage.scad>
use <x-axis-camera-spacer.scad>
use <x-axis-stop.scad>
use <x-axis-motor-mount.scad>
use <x-axis-pulley.scad>
use <x-axis-pulley-mount.scad>
use <x-axis-pulley-spacer.scad>
use <x-axis-roller.scad>
use <x-axis-roller-spacer.scad>
use <x-axis-top-carriage.scad>
use <y-axis-carriage.scad>
use <y-axis-wheel.scad>

// Module for assembling the parts
module assembly()
{
    // Left wheels
    translate([0, 35.5, 35.5])
    {
        rotate([0, 90, 0])
        {
            yAxisWheel();
        }
    }
    translate([0, 158.5, 35.5])
    {
        rotate([0, 90, 0])
        {
            yAxisWheel();
        }
    }
    // Left y axis carriage
    translate([30.25, 12, 12])
    {
        rotate([90, 0, 90])
        {
            yAxisCarriage();
        }
    }
    // X axis pulley
    translate([70.5, 51.75, 187])
    {
        rotate([0, 0, 90])
        {
            xAxisPulleyMount();
        }
    }
    translate([12.25, 102, 192])
    {
        xAxisPulleySpacer();
    }
    translate([12.25, 102, 196])
    {
        xAxisPulley();
    }
    // Extrusion dummy
    translate([40.5, 57, 161.75])
    {
        cube([1000, 80, 20]);
    }
    // X axis stop
    translate([110, 79.25, 161.75])
    {
        rotate([180, 0, 90])
        {
            xAxisStop();
        }
    }
    // X axis carriage
    translate([580.75, 34.25, 192.75])
    {
        rotate([0, 180, 0])
        {
            xAxisTopCarriage();
        }
    }
    translate([508, 147, 187.75])
    {
        rotate([180, 0, 0])
        {
            xAxisRollerSpacer();
        }
    }
    translate([508, 147, 166.75])
    {
        xAxisRoller();
    }
    translate([508, 147, 155.75])
    {
        xAxisRollerSpacer();
    }
    translate([540.5, 47, 187.75])
    {
        rotate([180, 0, 0]) {
            xAxisRollerSpacer();
        }
    }
    translate([540.5, 47, 166.75])
    {
        xAxisRoller();
    }
    translate([540.5, 47, 155.75])
    {
        xAxisRollerSpacer();
    }
    translate([573, 147, 187.75])
    {
        rotate([180, 0, 0])
        {
            xAxisRollerSpacer();
        }
    }
    translate([573, 147, 166.75])
    {
        xAxisRoller();
    }
    translate([573, 147, 155.75])
    {
        xAxisRollerSpacer();
    }
    translate([580.75, 34.25, 155.75])
    {
        rotate([0, 180, 0]) {
            xAxisBottomCarriage();
        }
    }
    // Camera spacers
    translate([526.5, 78, 138.75])
    {
        xAxisCameraSpacer();
    }
    translate([526.5, 106, 138.75])
    {
        xAxisCameraSpacer();
    }
    translate([554.5, 78, 138.75])
    {
        xAxisCameraSpacer();
    }
    translate([554.5, 106, 138.75])
    {
        xAxisCameraSpacer();
    }
    // C axis assembly
    translate([521, 108, 108.75])
    {
        cAxisMotorMount();
    }
    translate([540.5, 126, 108.5])
    {
        rotate([0, 180, 0]) {
            cAxisServoMount();
        }
    }
    translate([540.5, 126.75, 89.75])
    {
        rotate([90, 90, 180]) {
            cAxisServoArm();
        }
    }
    // X axis motor mount
    translate([1010.5, 51.75, 192])
    {
        rotate([0, 180, 270])
        {
            xAxisMotorMount();
        }
    }
    // Right y axis carriage
    translate([1050.75, 182, 12])
    {
        rotate([90, 0, 270])
        {
            yAxisCarriage();
        }
    }
    // Right wheels
    translate([1081, 35.5, 35.5])
    {
        rotate([0, 270, 0])
        {
            yAxisWheel();
        }
    }
    translate([1081, 158.5, 35.5])
    {
        rotate([0, 270, 0])
        {
            yAxisWheel();
        }
    }
}

// Using the module
assembly();
