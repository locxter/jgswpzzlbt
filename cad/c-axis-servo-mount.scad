// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module cAxisServoMount() {
  difference() {
    hull() {
      cylinder(12.5, 9.75, 9.75);
      translate([-15.25, 9, 12.5]) {
        cube([41.5, 5, 12.5]);
      }
    }
    translate([-16.25, -14, 12.5]) {
      cube([43.5, 23, 13.5]);
    }
    translate([0, 0, -1]) {
      cylinder(8.5, 2.75, 2.75);
    }
    translate([0, 0, 3.5]) {
      rotate([0, 90, 0]) {
        cylinder(17.75, 1.75, 1.75);
      }
    }
    translate([8.75, 0, 3.5]) {
      rotate([0, 90, 0]) {
        cylinder(8, 3, 3);
      }
    }
    translate([4.25, -3, -1]) {
      cube([3, 6, 8]);
    }
    translate([-6.25, 8, 12.5]) {
      cube([23.5, 7, 13.5]);
    }
    translate([-8.5, 8, 18.75]) {
      rotate([-90, 0, 0]) {
        cylinder(7, 1.75, 1.75);
      }
    }
    translate([19.5, 8, 18.75]) {
      rotate([-90, 0, 0]) {
        cylinder(7, 1.75, 1.75);
      }
    }
  }
}

cAxisServoMount();
