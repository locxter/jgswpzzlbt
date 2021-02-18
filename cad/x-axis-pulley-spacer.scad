// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisPulleySpacer () {
  difference() {
    cylinder(6, 3.75, 3.75);
    translate([0, 0, -1]) {
      cylinder(8, 2.75, 2.75);
    }
  }
}

xAxisPulleySpacer();