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
	// rosetas for M3 nuts
	difference() {
		union() {
			cube([5,BASE-5,5]); //
			//translate([0,BASE-5,0]) cube([15,5,5]); //
			translate([2,-20,0]) rotate([0,-90,0]) cube([15,20,2]);  // palheta
			translate([0,0,5]) cube([5,15,10]);
		}
		translate([5,0,-5]) cylinder(r=3,h=25); // curvatura
		translate([-5,15,15]) rotate([0,90,0]) cylinder(r=10,h=15); // curvatura
	}
}

module foot() {
	difference() {
		cylinder(h=5, d1=7, d2=6);
		translate([0,0,-1]) cylinder(h=8, d=3);
		translate([0,0, 1]) cylinder(h=8, d=5);
	}
}

module assembled_key()
{
		ARM_GAP = 2;
		EXPLODE = 10;

		// base
		translate([0,0,5]) color("Grey") base();

		// arms
		translate([BASE/2+ARM_GAP,0,5+8]) color("Red") arm();  				// right arm
		translate([BASE/2-ARM_GAP,0,5+8]) mirror([1,0,0]) color("Red") arm();  // left arm
		
		// 4 feet and 4 screws
		translate([BASE/2,BASE/2,5])		// center the base
		for(r=[0,90,180,270]) {
			rotate(a=r, v=[0,0,1]) {
				translate([BASE/2-5,BASE/2-5,-EXPLODE]) mirror([0,0,1]) color("Black")foot();  // feet
				translate([BASE/2-5,BASE/2-5,6+EXPLODE]) mirror([0,0,1]) color("Blue")nut_cavity(); // screws
			}
		}
}

assembled_key();
//arm();
