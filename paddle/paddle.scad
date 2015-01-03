// Morse paddle

// Base dimensions
$fn = 60;
BASE = 50;

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
	difference() {		// base with holes		
		cube([BASE,BASE,6]);
		translate([5,5,0]) nut_cavity();	// #1
	}
}

module arm()
{
	// Design rules: construct for 3D printing, then rotate
	difference() {
		union() {
			cube([5,30,5]);
			translate([0,30,0]) cube([15,5,5]);
		}
		translate([5,0,0]) cylinder(r=3,h=6);
	}
}

module foot() {
	cylinder(h=5, d1=5, d2=4);
}

module key()
{
		// add parameter to explode
		ARM_GAP = 2;
		EXPLODE = 0;

		// base
		base();

		// arms
		translate([BASE/2+ARM_GAP,0,8]) arm();  // right arm
		translate([BASE/2-ARM_GAP,0,8]) mirror([1,0,0]) arm();  // left arm
		
		// feet
		translate([5,5,-5-EXPLODE]) foot();
		translate([BASE-5,5,-5-EXPLODE]) foot();
		translate([5,BASE-5,-5-EXPLODE]) foot();
		translate([BASE-5,BASE-5,-5-EXPLODE]) foot();

		// feet screws
		translate([5,5,6+20]) mirror([0,0,1]) nut_cavity();
		translate([BASE-5,5,6+20]) mirror([0,0,1]) nut_cavity();
		translate([5,BASE-5,6+20]) mirror([0,0,1]) nut_cavity();
		translate([BASE-5,BASE-5,6+20]) mirror([0,0,1]) nut_cavity();
}

//nut_cavity();
//arm();
//base();	// base
key();
