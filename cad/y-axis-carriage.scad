// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module yAxisCarriage() {
  union() {
    difference() {
      hull() {
        cube([47, 47, 10]);
        translate([123, 0, 0]) {
          cube([47, 47, 10]);
        }
        translate([39.75, 65, 0]) {
          cube([90.5, 110, 10]);
        }
      }
      translate([8, 8, -1]) {
        cylinder(12, 1.75, 1.75);
      }
      translate([8, 8, -1]) {
        cylinder(6, 3, 3);
      }
      translate([39, 8, -1]) {
        cylinder(12, 1.75, 1.75);
      }
      translate([39, 8, -1]) {
        cylinder(6, 3, 3);
      }
      translate([8, 39, -1]) {
        cylinder(12, 1.75, 1.75);
      }
      translate([8, 39, -1]) {
        cylinder(6, 3, 3);
      }
      translate([39, 39, -1]) {
        cylinder(12, 1.75, 1.75);
      }
      translate([39, 39, -1]) {
        cylinder(6, 3, 3);
      }
      translate([23.5, 23.5, -1]) {
        cylinder(12, 11.25, 11.25);
      }
      translate([146.5, 23.5, -1]) {
        cylinder(12, 5, 5);
      }
      translate([146.5, 23.5, 4.5]) {
        cylinder(6.5, 8, 8);
      }
    }
    translate([39.75, 124.5, 10]) {
      difference() {
        cube([90.5, 50.5, 30.5]);
        translate([-1, 0, 0]) {
          rotate([45, 0, 0]) {
            cube([92.5, sqrt(800), sqrt(800)]);
          }
        }
        translate([-1, -1, 20]) {
          cube([92.5, 21, 11.5]);
        }
        translate([5, 25, -1]) {
          cube([80.5, 20.5, 32.5]);
        }
        translate([15.25, -1, 10.25]) {
          rotate([-90, 0, 0]) {
            cylinder(52.5, 2.75, 2.75);
          }
        }
        translate([75.25, -1, 10.25]) {
          rotate([-90, 0, 0]) {
            cylinder(52.5, 2.75, 2.75);
          }
        }
        translate([15.25, -1, 10.25]) {
          rotate([-90, 0, 0]) {
            cylinder(21, 4.5, 4.5);
          }
        }
        translate([75.25, -1, 10.25]) {
          rotate([-90, 0, 0]) {
            cylinder(21, 4.5, 4.5);
          }
        }
      }
    }
  }
}

yAxisCarriage();
