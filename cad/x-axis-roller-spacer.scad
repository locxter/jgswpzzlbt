// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisRollerSpacer () {
  difference() {
    hull() {
      cylinder(5.5, 7.75, 7.75);
      translate([0, 0, 5.5]) {
        cylinder(5.5, 3.75, 3.75);
      }
    }
    translate([0, 0, -1]) {
      cylinder(13, 2.75, 2.75);
    }
  }
}

xAxisRollerSpacer();