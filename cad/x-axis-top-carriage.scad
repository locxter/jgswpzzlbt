// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisTopCarriage() {
  difference() {
    hull() {
      translate([25.75, 0, 0]) {
        cube([15.5, 25.5, 5]);
      }
      translate([0, 61, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([51.5, 61, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([0, 105, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([51.5, 105, 0]) {
        cube([15.5, 15.5, 5]);
      }
    }
    hull() {
      translate([33.5, 7.75, -1]) {
        cylinder(7, 2.75, 2.75);
      }
      translate([33.5, 17.75, -1]) {
        cylinder(7, 2.75, 2.75);
      }
    }
    translate([7.75, 78.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([59.25, 68.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([7.75, 112.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([59.25, 112.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
  }
}

xAxisTopCarriage();
