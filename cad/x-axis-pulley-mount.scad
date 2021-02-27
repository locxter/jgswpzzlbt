// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisPulleyMount() {
  difference() {
    hull() {
      cube([90.5, 40.5, 5]);
      translate([41, 40.5, 0]) {
        cube([18.5, 35.5, 5]);
      }
    }
    translate([15.25, 20.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([75.25, 20.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    hull() {
      translate([50.25, 48.25, -1]) {
        cylinder(7, 2.75, 2.75);
      }
      translate([50.25, 68.25, -1]) {
        cylinder(7, 2.75, 2.75);
      }
    }
  }
}

xAxisPulleyMount();
