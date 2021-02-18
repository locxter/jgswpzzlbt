// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisRollerSpacer () {
  difference() {
    cylinder(13, 3.75, 3.75);
    translate([0, 0, -1]) {
      cylinder(15, 2.75, 2.75);
    }
  }
}

xAxisRollerSpacer();