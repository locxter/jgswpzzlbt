// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisPulleySpacer () {
  difference() {
    hull() {
      cylinder(2, 7.75, 7.75);
      translate([0, 0, 2]) {
        cylinder(2, 3.75, 3.75);
      }
    }
    translate([0, 0, -1]) {
      cylinder(6, 2.75, 2.75);
    }
  }
}

xAxisPulleySpacer();