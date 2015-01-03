// Morse paddle

// Base dimensions
$fn = 60;
BASE = 60;

module nut_cavity() {
	// Nut dimensions
	m3nutd = 5; // nut diameter
	m3nuth = 3; // nut height
	m3hole = 3; // hole diameter

	union(){
		cylinder(r=m3hole/2,h=8); // screw hole
		rotate(a=30) cylinder(r=m3nutd/2,h=1+m3nuth/2,$fn=6); // screw nut cavity
	}
}

module base()
{
	// base with holes
	difference() {				
		cube([BASE,BASE,6]);
		// hex nut holes
		translate([5,5,0+6]) mirror([0,0,1]) nut_cavity();	// #1
		translate([BASE-5,5,0+6]) mirror([0,0,1]) nut_cavity();	// #2
		translate([5,BASE-5,0+6]) mirror([0,0,1]) nut_cavity();	// #3
		translate([BASE-5,BASE-5,0+6]) mirror([0,0,1]) nut_cavity();	// #4
	}
}

module arm()
{
	// Design rules: construct for 3D printing, then rotate
	difference() {
		union() {
			cube([5,BASE-5,5]); //
			translate([0,BASE-5,0]) cube([15,5,5]); //
			translate([2,-20,0]) rotate([0,-90,0]) cube([15,20,2]);  // palheta
		}
		translate([5,0,0]) cylinder(r=3,h=6); // curvatura
	}
}

module foot() {
	cylinder(h=5, d1=5, d2=4);
}

module key()
{
		ARM_GAP = 2;
		EXPLODE = 0;

		// base
		base();

		// arms
		translate([BASE/2+ARM_GAP,0,8]) color("Red") arm();  				// right arm
		translate([BASE/2-ARM_GAP,0,8]) mirror([1,0,0]) color("Red") arm();  // left arm
		
		// 4 feet and 4 screws
		translate([BASE/2,BASE/2,0])		// center in base
		for(r=[0:4]) {
			rotate([0,0,r*360/4]) {
				translate([BASE/2-5,BASE/2-5,-EXPLODE]) mirror([0,0,1]) color("Black")foot();  // feet
				translate([BASE/2-5,BASE/2-5,6+EXPLODE]) mirror([0,0,1]) color("Grey")nut_cavity(); // screws
			}
		}
}

key();

