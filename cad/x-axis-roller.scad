// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisRoller() {
  difference() {
    union() {
      cylinder(2.75, 10, 10);
      translate([0, 0, 2.75]) {
        cylinder(4.5, 13, 13);
      }
      translate([0, 0, 7.25]) {
        cylinder(2.75, 10, 10);
      }
    }
    translate([0, 0, -1]) {
      cylinder(12, 8, 8);
    }
  }
}

xAxisRoller();
