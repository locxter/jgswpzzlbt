// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;  

module xAxisMotorMount() {
  difference() {
    hull() {
      cube([90.5, 60.5, 5]);
      translate([23, 60.5, 0]) {
        cube([44.5, 44.5, 5]);
      }
    }
    translate([15.25, 20.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([75.25, 20.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([29.75, 67.25, -1]) {
      cylinder(7, 1.75, 1.75);
    }
    translate([60.75, 67.25, -1]) {
      cylinder(7, 1.75, 1.75);
    }
    translate([29.75, 98.25, -1]) {
      cylinder(7, 1.75, 1.75);
    }
    translate([60.75, 98.25, -1]) {
      cylinder(7, 1.75, 1.75);
    }
    translate([45.25, 82.75, -1]) {
     cylinder(7, 11.25, 11.25);
    }
  }
}

xAxisMotorMount();
