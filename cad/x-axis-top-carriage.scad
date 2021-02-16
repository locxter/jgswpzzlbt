// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisTopCarriage() {
  difference() {
    hull() {
      translate([25.75, 0, 0]) {
        cube([15.5, 25.5, 5]);
      }
      translate([0, 62.5, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([51.5, 62.5, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([0, 109, 0]) {
        cube([15.5, 15.5, 5]);
      }
      translate([51.5, 109, 0]) {
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
    translate([7.75, 70.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([59.25, 70.25, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([7.75, 116.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([59.25, 116.75, -1]) {
      cylinder(7, 2.75, 2.75);
    }
    translate([64, 91, -1]) {
      cylinder(7, 1.75, 1.75);
    }
    translate([64, 110, -1]) {
      cylinder(7, 1.75, 1.75);
    }
  }
}

xAxisTopCarriage();
