// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module yAxisWheel() {
  difference() {
    union() {
      cylinder(3, 35.5, 35.5);
      translate([0, 0, 3]) {
        cylinder(6, 35, 35);
      }
      translate([0, 0, 9]) {
        cylinder(3, 35.5, 35.5);
      }
      translate([0, 0, 12]) {
        cylinder(6, 35, 35);
      }
      translate([0, 0, 18]) {
        cylinder(3, 35.5, 35.5);
      }
      translate([0, 0, 21]) {
        cylinder(6, 35, 35);
      }
      translate([0, 0, 27]) {
        cylinder(3, 35.5, 35.5);
      }
    }
    translate([0, 0, 15.5]) {
      cylinder(15.5, 2.75, 2.75);
    }
    translate([0, 0, 22.75]) {
      rotate([0, 90, 0]) {
        cylinder(36.5, 1.75, 1.75);
      }
    }
    translate([9.75, 0, 22.75]) {
      rotate([0, 90, 0]) {
        cylinder(26.75, 3, 3);
      }
    }
    translate([4.75, -3, 19.25]) {
      cube([3, 6, 11.75]);
    }
  }
}

yAxisWheel();
