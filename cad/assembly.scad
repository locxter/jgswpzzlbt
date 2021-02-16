// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

use <c-axis-motor-mount.scad>
use <c-axis-servo-arm.scad>
use <c-axis-servo-mount.scad>
use <x-axis-bottom-carriage.scad>
use <x-axis-camera-spacer.scad>
use <x-axis-motor-mount.scad>
use <x-axis-pulley-mount.scad>
use <x-axis-pulley-spacer.scad>
use <x-axis-roller-spacer.scad>
use <x-axis-top-carriage.scad>
use <y-axis-carriage.scad>
use <y-axis-wheel.scad>

module assembly() {
  translate([0, 35.5, 35.5]) {
    rotate([0, 90, 0]) {
      yAxisWheel();
    }
  }
  translate([0, 158.5, 35.5]) {
    rotate([0, 90, 0]) {
      yAxisWheel();
    }
  }
  translate([30.5, 12, 12]) {
    rotate([90, 0, 90]) {
      yAxisCarriage();
    }
  }
  translate([71, 51.75, 187]) {
    rotate([0, 0, 90]) {
      xAxisPulleyMount();
    }
  }
  translate([12.75, 97, 192]) {
    xAxisPulleySpacer();
  }
  translate([41, 57, 161.75]) {
    cube([1000, 80, 20]);
  }
  translate([507.5, 34.75, 189.75]) {
    xAxisTopCarriage();
  }
  translate([541, 47.5, 174.75]) {
    xAxisRollerSpacer();
  }
  translate([541, 47.5, 153.75]) {
    xAxisRollerSpacer();
  }
  translate([515.25, 151.5, 174.75]) {
    xAxisRollerSpacer();
  }
  translate([515.25, 151.5, 153.75]) {
    xAxisRollerSpacer();
  }
  translate([566.75, 151.5, 174.75]) {
    xAxisRollerSpacer();
  }
  translate([566.75, 151.5, 153.75]) {
    xAxisRollerSpacer();
  }
  translate([574.5, 34.75, 153.75]) {
    rotate([0, 180, 0]) {
      xAxisBottomCarriage();
    }
  }
  translate([527, 83, 136.75]) {
    xAxisCameraSpacer();
  }
  translate([527, 111, 136.75]) {
    xAxisCameraSpacer();
  }
  translate([555, 83, 136.75]) {
    xAxisCameraSpacer();
  }
  translate([555, 111, 136.75]) {
    xAxisCameraSpacer();
  }
  translate([523, 112.5, 89.25]) {
    cAxisMotorMount();
  }
  translate([541, 130.5, 88.75]) {
    rotate([0, 180, 0]) {
      cAxisServoMount();
    }
  }
  translate([541, 131.25, 70]) {
    rotate([90, 90, 180]) {
      cAxisServoArm();
    }
  }
  translate([1011, 142.25, 187]) {
    rotate([0, 0, 270]) {
      xAxisMotorMount();
    }
  }
  translate([1051.5, 182, 12]) {
    rotate([90, 0, 270]) {
      yAxisCarriage();
    }
  }
  translate([1082, 35.5, 35.5]) {
    rotate([0, 270, 0]) {
      yAxisWheel();
    }
  }
  translate([1082, 158.5, 35.5]) {
    rotate([0, 270, 0]) {
      yAxisWheel();
    }
  }
}

assembly();
