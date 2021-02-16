// Uncomment before final render
//$fa = 0.01;
//$fs = 0.25;

module xAxisBottomCarriage() {
  union() {
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
      translate([19.5, 48.25, -1]) {
        cylinder(7, 1.25, 1.25);
      }
      translate([19.5, 76.25, -1]) {
        cylinder(7, 1.25, 1.25);
      }
      translate([47.5, 76.25, -1]) {
        cylinder(7, 1.25, 1.25);
      }
      translate([47.5, 48.25, -1]) {
        cylinder(7, 1.25, 1.25);
      }
      translate([7.75, 116.75, -1]) {
        cylinder(7, 2.75, 2.75);
      }
      translate([59.25, 116.75, -1]) {
        cylinder(7, 2.75, 2.75);
      }
    }
    translate([15.5, 109.5, 5]) {
      difference() {
        cube([36, 15, 50]);
        translate([-1, 0, 0]) {
          rotate([45, 0, 0]) {
            cube([41, sqrt(200), sqrt(200)]);
          }
        }
        translate([-1, -1, 10]) {
          cube([41, 11, 41]);
        }
        hull() {
          translate([7.75, 9, 17.75]) {
            rotate([-90, 0, 0]) {
              cylinder(7, 2.75, 2.75);
            }
          }
          translate([7.75, 9, 42.25]) {
            rotate([-90, 0, 0]) {
              cylinder(7, 2.75, 2.75);
            }
          }
        }
        hull() {
          translate([28.25, 9, 17.75]) {
            rotate([-90, 0, 0]) {
              cylinder(7, 2.75, 2.75);
            }
          }
          translate([28.25, 9, 42.25]) {
            rotate([-90, 0, 0]) {
              cylinder(7, 2.75, 2.75);
            }
          }
        }
      }
    }
  }
}

xAxisBottomCarriage();
