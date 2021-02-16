// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisCameraSpacer() {
  difference() {
    cylinder(12, 2.25, 2.25);
    translate([0, 0, -1]) {
      cylinder(14, 1.25, 1.25);
    }
  }
}

xAxisCameraSpacer();