//@==================================================================//
//@ Comment notes:                                                   //
//@   // or /*        - Keith's comments.                            //
//@   crtl+f: @       - Syrian's comments.                           //
//@   crtl+f: $       - Syrian's comments + questions.               //
//@   crtl+f: ~       - Syrian's comments + flagged for removal/mod. //
//@   crtl+f: RELOOK  - Relook at before compiling                   //
//@==================================================================//

//@==================================================================//
//@ Modification notes:
//@		1. Started changing n3's to nx's, xyz1 and xyzi to x1, and 
//@			omitting ny, nz, xyz2, xyz3, disy, disz
//@		2. Removed random number generator parts and using MTRand.
//@		3. openlog(), closelog() functions.
//@   4. readconf() not used because doesn't seem like we'd want to?
//@   5. removed quite a bit
//@   6. reorganized, temp loop, reset to 0 start of each temp loop,
//@      put rng to reset start of each temp loop, <M^2>, <M^4>, <E>,
//@			 p=10, isteps, msteps, and EnVsisteps (calcenrcool) 
//@==================================================================//

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>

#include "MT/mtrand.h"

using namespace std;



//@----------------------------------------------------------------
//@ Declare (and initialize some) variables
//@----------------------------------------------------------------



const double power = 1.5;

//@ power of ising interactions decay 

//const double avn1ctemp = 1.0;

//@ for first "extra" idea


const int nx = 512;

//@ size in x-direction (since 1d, it's only direction)

const int ll = 10000;

//@ max value for expansion truncation L (hamiltonian string cut off max)

const int lls = 500;

//$ maximum length of subsequence

const double ht = 0.5;

// transverse field strength

const double start_temp = 0.5;

const double temp_step = 0.01;

const int num_temp_step = 500;

const double final_temp = start_temp + temp_step*(num_temp_step - 1);


long int msteps = 6400000;

//@ measurement steps

long int isteps = 2000;

//@ equilibriation steps


int l;

//@ expansion truncation, or hamiltonian string cut off max

int nh;

//@ current expansion order of n, or number of non-identity hamiltonians in hamiltonian string

int mlls;

//@ maximum substring length that has been reached

int nmsr;

// number of measurements counted (for getting averages)

double temp;

double beta;

// inverse temperature

long double ar1;

long double ar2;

long double ar3;

long double ar4;

//@ acceptance rates

long double eshft;

/*$ sum of all constants added to the hamiltonian (see hamiltonian equations perhaps?) */

long double su;

//$

long double xu;

//$

long double avnu;

//$

long double avni;

//$

long double avnt;

//$

long double avn1; 

//@ average energy perhaps

long double avn2;

//$


long double av_magpow2;

long double av_magpow4;

//@ <M^2> and <M^4>



//@----------------------------------------------------------------
//@ Declare/initialize arrays
//@----------------------------------------------------------------



int spn[nx+1] = {0};

//@ spin array

int stra[ll+1] = {0};

int strb[ll+1] = {0};

/*~ (NOTE reason for names, see str1 below) operator string(s)? uses ll which is the max expansion truncation. go down p list decides which operator it is. i and j can be much farther apart, but there are still two things to specify hence the 1,2 part of the multidimensions first array gives i second array gives j put together to get Hij. */

int x1[nx+1] = {0}; //@ changed xyz1 to x1

/*~ corrdinates for given spin i*/

int disx[nx+1][nx+1] = {0};

/*~ represents the periodic distances in x directions between two points*/

int pfrst[nx+1] = {0};

/*$ first position to search in probability list given INT(R*n3), what is R (random number generator?) and is this similar to frstspinop array? */

int plast[nx+1] = {0};

/*$ last position to search in probability list given INT(R*n3), what is R (random number generator?) and is this similar to lastspinop array? */

bool lis[3][3][nx/2 + 1] = {0};

/*$ //~ For allowed spin configuration given distance between sites original was LOGICAL lis(-1:1,-1:1,0:nx/2,0:ny/2,0:nz/2)*/

double ris[nx/2 + 1] = {0};

/*$ //~ ising interaction with constants added given distance */

double pint[nx + 1] = {0};

/*$ //~ list of integrated probabilities lists */

double ap1[ll + 1] = {0};

/*$ probability for increasing n by 1, given current n. expansion order n (number of non identity hamiltonians in string). */

double dp1[ll + 1] = {0};

/*$ probability for decreasing n by 1, given current n. expansion order n (number of non identity hamiltonians in string). */

int lsub[nx+1] = {0};

/*$ //~ length of subsequences?*/

int pos1[lls+1][nx+1] = {0};

/*$ //~ positions?*/

int str1[lls+1][nx+1] = {0};

//$ //~ substring operators? (NOTE above name stra and strb arrays)

bool con1[lls+1][nx+1] = {0};

//$ //~ constraints? watch the math

int pos[lls+1] = {0};

//$

int opr[lls+1] = {0};

//$

int tmp1[ll+1] = {0};

//$

int tmp2[ll+1] = {0};

//$

bool lstr[ll+1] = {0};

//$

int spn1[nx+1] = {0};

//$



//@----------------------------------------------------------------
//@ Declare functions within main
//@----------------------------------------------------------------



void init(MTRand_open& rand_num);

//@ initializes the system: initconf, lattice, pvectors, isingpart, zerodat

void mcstep(MTRand_open& rand_num);

//@ monte carlo step

void checkl(long int step, ofstream& myfile1, MTRand_open& rand_num);

/*$ check the hamiltonian string length perhaps? has pvectors, openlog, and closelog */

void errcheck();

void writeconf(ofstream& myfile2);

/*$ writes length of lattice in x, beta, L (hamiltonian string length?), spn(i) spin array values, str(1,i) (bond array indicates site left for our case?), str(2,i) (bond array indicates site right for our case?)*/ 

void measure();

/*$ measuring observables looks like. note: uses a common which seems like fortran77's version of a sort of pass by reference across different fortran .f files*/

void results(ofstream& myfile3, ofstream& myfile4, ofstream& myfile6, ofstream& myfile7, ofstream& myfile8, ofstream& myfile9);

/*@ writes calculated observables into (10) mag.dat for su and xu, (10) enr.dat for e, c, avni, avnt, and avnu */

void writeacc(ofstream& myfile5);

/*$ writes diagonal acceptance 1, diagonal acceptance 2, off-diagonal substitutions, spin flips / site, max lsub (max hamiltonian string length?) */

//void calcenrcool (int step, ofstream& myfile8);

//@



//@----------------------------------------------------------------
//@ Declare functions within functions
//@----------------------------------------------------------------



void initconf(MTRand_open& rand_num);

/*@ generates initial random spin state and sets initial values for l (Hamiltonian string length) and nh (number of non identity Hamiltonians) */

void lattice();

/*$ creates lattice for coordinates for the lattice sites (1D in this case though) and vectors for distances that take into account the periodic boundary conditions (gives shorter of two possible distances, although this is probably simpler because we're just 1d) */

void pvectors();

/*$ calculate the acceptance probabilities for the n-changing update */

void isingpart();

/*$ not entirely sure what this does */

void zerodat();

//@ sets measurement data variables to 0.0

void diaupdate(MTRand_open& rand_num);

//@ [-1,i] = flips i; [0,0] = 1; [i,i] = h; [i,j] = ising operator

void partition();

//$ ?

void offupdate(int s, MTRand_open& rand_num);

//$



//@----------------------------------------------------------------
//@ Main function
//@----------------------------------------------------------------



int main() {

	ofstream writelog; //@ Initialize the write-data-to-file.

	writelog.open ("logN512_task3_run3.txt"); //@ Name and begin the write-data-to-file.

	ofstream writeconfig;

	writeconfig.open ("writeconfN512_task3_run3.txt");

	ofstream writemag;

	writemag.open ("magN512_task3_run3.txt");

	ofstream writeenr;

	writeenr.open ("enrN512_task3_run3.txt");

	ofstream writeac;

	writeac.open ("accN512_task3_run3.txt");

	ofstream writeavn1_temp;

	writeavn1_temp.open ("avn1vstempN512_task3_run3.txt");

	ofstream writemag_2vstemp;

	writemag_2vstemp.open ("mag^2vstempN512_task3_run3.txt");

	ofstream writeevstemp;

	writeevstemp.open ("EvsTempN512_task3_run3.txt");

	ofstream writebindercumulantvstemp;

	writebindercumulantvstemp.open ("bindercumulantvstempN512_task3_run3.txt");
	
	for (int y=0; y<num_temp_step; y++) {

  	temp = start_temp + y*temp_step;
		
		//@ start temp loop

    beta = 1.0/temp;

    cout << "Now processing T = " << temp << ", beta = " << beta << "\n\n";

    writelog << "Now processing T = " << temp << ", beta = " << beta << "\n\n";

   	writeavn1_temp << temp << " ";

		writemag_2vstemp << temp << " ";

		writebindercumulantvstemp << temp << " ";

		writeevstemp << temp << " ";

		unsigned long int time_seed = time(NULL);

		MTRand_open ran(time_seed);

		//@ random number generator (0,1). Note: omitted RAN(), INITRANDOM, and WRITERAND(rndf)

		mlls = 0;

		ar1 = 0.0;

		ar2 = 0.0;

		ar3 = 0.0;

 		ar4 = 0.0;

		/*@ Important: setting variables that need to be 0 (or 0.0 if a double) to that between temperatures!!*/

		for (int c=0; c<(ll+1); c++) {

			ap1[c] = 0.0;

			dp1[c] = 0.0;

			tmp1[c] = 0;

			tmp2[c] = 0;

			lstr[c] = 0;

		}

		for (int d=0; d<(nx+1); d++) {

			pfrst[d] = 0;

			plast[d] = 0;

			lsub[d] = 0;

			spn1[d] = 0;

			for (int f=0; f<(lls+1); f++) {

				pos1[f][d] = 0;

				str1[f][d] = 0;

				con1[f][d] = 0;

				pos[f] = 0;

				opr[f] = 0;

			}

		}

		/*@ Important: setting arrays that need to be 0 (or 0.0 if a double) to that between temperatures!!*/
		//@****************** check all variables first use

		init(ran);

		for (long int i=1; i<(isteps+1); i++) {

			mcstep(ran);

			checkl(i, writelog, ran);

			//if (temp==avn1ctemp){

				//calcenrcool(i, writeavn1c_cool);

			//}
			
			if ((i%(isteps/10))==0) { //@ to record every 10 steps

				writelog << "Done equilibriation step: " << i << "\n\n";	

				errcheck();

			}

		}

		writeconf(writeconfig);

		ar1 = 0.0;
		ar2 = 0.0;
		ar3 = 0.0;
		ar4 = 0.0;

		for (long int k=1; k<(msteps+1); k++) {

			mcstep(ran);

			if ((k%2)==0) {

				measure();

			}

			if ((k%(msteps/10))==0) { //@ to record every 10 steps

				writelog << "Done measurement step " << k << "\n\n";

				errcheck();

			}

		}

		results(writemag, writeenr, writeavn1_temp, writemag_2vstemp, writeevstemp, writebindercumulantvstemp);

		writeacc(writeac);

		writeconf(writeconfig);
	
		cout << "Completed temp step of temp = " << temp << ", beta = " << beta << "\n\n";

		writelog << "Completed temp step of temp = " << temp << ", beta = " << beta << "\n\n";

	}

	writelog.close();

	writeconfig.close();

	writemag.close();

	writeenr.close();

	writeac.close();

	writeavn1_temp.close();

	writemag_2vstemp.close();

	writeevstemp.close();

	writebindercumulantvstemp.close();

	cout << "Done.\n\n";

	return 0;


}



//@----------------------------------------------------------------
//@ Helper functions
//@----------------------------------------------------------------



void init(MTRand_open& rand_num) {

	initconf(rand_num);

	lattice();

	pvectors();

	isingpart();

	zerodat();


}



//@----------------------------------------------------------------



void initconf(MTRand_open& rand_num) {

	/*@ Generates the initial random spin state, and sets initial values
	for l (expansion truncation) and nh (current expansion order n). */

	int c;
		
	l = 20;

	nh = 0;

	for (int i=1; i<(ll+1); i++) {

		stra[i] = 0;

		strb[i] = 0;

	}
	
	for (int j=1; j<(nx+1); j++) {

		spn[j] = -1;

	}

	//@ setting all spins to -1 first, could use the fill_n thing, but I'll leave it as is at least for now

	for (int k=1; k<(nx/2 + 1); k++) { 

		c = min((int(rand_num()*nx) + 1), nx);
		
		while (true) {

			if (spn[c] == -1) {

				break;

			}

			c = min ((int(rand_num()*nx) + 1), nx);

		}

		// the result of this is to randomly select half of the spins and flip them to +1, seems like an overly complicated way to initialize the spins, but whatever

		spn[c] = 1;

	}

	
}



//@----------------------------------------------------------------



void lattice() {

		/*@Creates maps with coordinates for the lattice sites "(xyz, xyzi)", 
	and vectors disx,"disy,disz" for distances that take into account the 
	periodic boundary conditions (gives the shorter of the two possible
	distances).*/

	// many of these data structures may no longer be necessary/useful in 1D code, but possibly useful to hold on to for higher dimensional generalization	
	
	int i = 0;

	for (int ix=1; ix<(nx+1); ix++) {

		i = i + 1;		

		x1[i] = ix;

	}
	
	// object below encodes distances on a periodic 1D lattice

	for (int j=1; j<(nx+1); j++) {

		for (int k=1; k<(nx+1); k++) {

			disx[k][j] = abs(k - j);

			if ((disx[k][j])>(nx/2)) {

				disx[k][j] = nx - disx[k][j]; /*@ think 1D chain with PBC so it's like a ring which means max disx is nx/2 */

			}

		}

	}


}



//@----------------------------------------------------------------



void pvectors() {

	//@ Calculates the acceptance probabilities for the n-changing update.

	for (int i=0; i<l; i++ ) {

		ap1[i] = ht * beta * double(nx) / (double(l - i));

	} 

	for (int j=1; j<(l+1); j++) {

		dp1[j] = double(l - j + 1) / (ht * beta * double(nx));

	}


}



//@----------------------------------------------------------------



void isingpart() {

	int ix; //@note ix used for inside below loop as well
	
	double r1, r2, p1, p2;

	for (ix=0; ix<(nx/2 + 1); ix++) { /*$ what do these exactly accomplish*/

		if (ix==0) {

			ris[ix] = ht;

			lis[2][2][ix] = true;

			lis[2][0][ix] = false;

			lis[0][2][ix] = false;

			lis[0][0][ix] = true;

		} else {

			r1 = double(ix);

			r2 = double(nx - ix);

			ris[ix] = (-1.0)/pow(r1, power) - 1.0/pow(r2, power); //$~ TWEAK THIS FOR PROPER "DISTANCES OR THIS THING LOOK AT THOSE r^2's"

			if (ris[ix]<=0) {

				lis[2][2][ix] = true;

				lis[2][0][ix] = false;

				lis[0][2][ix] = false;

				lis[0][0][ix] = true;

			} else {

				lis[2][2][ix] = false;

				lis[2][0][ix] = true;

				lis[0][2][ix] = true;

				lis[0][0][ix] = false;

			}

			ris[ix] = abs(ris[ix]);

		}

	}

	pint[0] = 0.0;

	pint[1] = ris[0];

	eshft = 0.0;

	for (int i=2; i<(nx+1); i++) {

		ix = disx[1][ x1[i] ]; /*@ to be clear, this is finding the distance between the first site (site x1[0]) and every other site (site x1[i]), note: 1 to nx*/

		pint[i] = pint[i-1] + ris[ix];

		eshft = eshft + (ris[ix]/2);

	}
	
	for (int j=1; j<(nx+1); j++) {

		pint[j] = pint[j]/pint[nx];

	}

	for (int k=0; k<(nx+1); k++) {

		p1 = double(k)/double(nx);

		p2 = double(k+1)/double(nx);

		if (k==0) {

			pfrst[k] = 1;

		} else {

			for (int c=pfrst[k-1]; c<(nx+1); c++) {

				if (pint[c]>=p1) {

					pfrst[k] = c;

					break; //@ breaks and goes to below "if (k==nx)" statement

				}

			}

		}

		if (k==nx) {

			plast[k] = nx;

		} else {

			for (int d=pfrst[k]; d<(nx+1); d++) {

				if (pint[d]>=p2) {

					plast[k] = d;

					break; /*@ breaks and continues "for (int k=0; k<(nx+1); k++)" loop */
				}

			}

		}

	}


}



//@----------------------------------------------------------------



void mcstep(MTRand_open& rand_num) {

	diaupdate(rand_num);

	partition();

	for (int i=1; i<(nx+1); i++) {

		offupdate(i, rand_num);

	}


}



//@----------------------------------------------------------------



void diaupdate(MTRand_open& rand_num) {

	/*@ [-1,i] = flips i
			[0,0]  = 1
			[i,i]  = h
			[i,j]  = Ising operator */

	int s1, s2, p0, p1, p2, ix, nac1, nac2, ntr2;

	double p;

	bool failed_all_ifs = false;

	nac1 = 0;

	nac2 = 0;

	ntr2 = 0;

	for (int i=1; i<(l+1); i++) {

		s1 = stra[i];

		s2 = strb[i];

		if (s1==0) { //@ H00

			p = ap1[nh];

			if (p>=1) {

				stra[i] = min(int(rand_num()*nx)+1, nx);

				strb[i] = stra[i];

				nh = nh + 1;

				nac1 = nac1 + 1;

			} else if (rand_num()<p) {

				stra[i] = min(int(rand_num()*nx)+1, nx);

				strb[i] = stra[i];

				nh = nh + 1;

				nac1 = nac1 + 1;

			}
			
		} else if (s1==s2) { //@ Hii

			p = dp1[nh];

			if (p>=1) {

				stra[i] = 0;

				strb[i] = 0;

				nh = nh - 1;

				nac1 = nac1 + 1;

			} else if (rand_num()<p) {

				stra[i] = 0;

				strb[i] = 0;

				nh = nh - 1;

				nac1 = nac1 + 1;

			}
					
		} else if (s1==-1) { //@ H(-1)j

			spn[s2] = -spn[s2];

		}
					
		s1 = stra[i];
		/*~ Can likely omit this considering he already defined s1 to this and didn't change s1 inbetween this. */						

		if (s1>0) { //@ doesn't this work on Hii as well if i>0

			while (true) {

				if (!failed_all_ifs) {

					ntr2 = ntr2 + 1;

					p = rand_num();

					p0 = int(p*nx);

					p1 = pfrst[p0];

					p2 = plast[p0];

				} else {

				failed_all_ifs = false;

				//@ for last branch with an if, else statement

				}

				if (p1==p2) {

					s2 = ((s1 + p1 - 2) % nx) + 1;

					ix = disx[ x1[s1] ][ x1[s2] ];

					if ((lis[spn[s1]+1][spn[s2]+1][ix])) {

						strb[i] = s2;

						nac2 = nac2 + 1;

						break;

					} else {

						continue;

					}

				} else if (p2==(p1+1)) {

					if (pint[p1]>=p) {

						s2 = ((s1 + p1 - 2) % nx) + 1;

					} else {

						s2 = ((s1 + p2 - 2) % nx) + 1;

					}

					ix = disx[ x1[s1] ][ x1[s2] ];		

					if (lis[ spn[s1]+1 ][ spn[s2]+1 ][ix]) {

						strb[i] = s2;

						nac2 = nac2 + 1;

						break;

					} else {

						continue;

					}

				} else {

					p0 = (p1 + p2)/2;

					if ((pint[p0-1]<p)&&(pint[p0]>=p)) {

						s2 = ((s1 + p0 - 2) % nx) + 1;

						ix = disx[ x1[s1] ][ x1[s2] ];

						if (lis[ spn[s1]+1 ][ spn[s2]+1 ][ix]) {

							strb[i] = s2;

							nac2 = nac2 + 1;

							break;

						} else {

							continue;

						}

					} else {

						if ((pint[p0])>=p) {

							p2 = p0;

						} else {

							p1 = p0;

						}

						failed_all_ifs = true;

						continue;

						//@ see "near" beginning of while(true) statement

					}

				}	

			}

		}

	}

	ar1 = ar1 + double(nac1)/double(l);

	if (ntr2!=0) {

		ar2 = ar2 + double(nac2)/double(ntr2);

	}


}



//@----------------------------------------------------------------



void partition() {

	int s1, s2;
	
	for (int i=1; i<(nx+1); i++) {

		lsub[i] =0;

		con1[0][i] = false;

	}

	for (int j=1; j<(l+1); j++) {

		s1 = stra[j];

		if (s1!=0) {

			s2 = strb[j];

			if (s1==s2) {

				lsub[s1] = lsub[s1] + 1;

				pos1[ lsub[s1] ][s1] = j;

				str1[ lsub[s1] ][s1] = 1;

				con1[ lsub[s1] ][s1] = false;

			} else if (s1==(-1)) {

				lsub[s2] = lsub[s2] + 1;

				pos1[ lsub[s2] ][s2] = j;

				str1[ lsub[s2] ][s2] = 2;

				con1[ lsub[s2] ][s2] = false;

			} else {

				con1[ lsub[s1] ][s1] = true;

				con1[ lsub[s2] ][s2] = true;

			}

		}

	}
	
	for (int k=1; k<(nx+1); k++) {

		if (con1[0][k]) {

			con1[ lsub[k] ][k] = true;

		}
			
		if (lsub[k]>mlls) {

			mlls = lsub[k];

		}

	}


}



//@----------------------------------------------------------------



void offupdate(int s, MTRand_open& rand_num) {
	
	int ii, p1, p2, lens, flp, top, nac, nupd;

	nupd = 0;

	lens = lsub[s];

	for (int i=1; i<(lens+1); i++) {

		if ((!con1[i][s])) {

			nupd = nupd + 1;

			pos[nupd] = i;

		}

		opr[i] = str1[i][s];

	}

	if (lens<2) {

		if ((nupd == lens)&&((!con1[0][s]))) { //$ ?

			if (rand_num()<0.75) {

				spn[s] = -spn[s];

				ar4 = ar4 + 1.0;

				return;

			} else {

				return;

			}

		} else {

			return;

		}

	}

	nac = 0;

	flp = 1;

	for (int j=1; j<(2*nupd); j++) {

		p1 = min(int(rand_num()*nupd)+1, nupd);

		p1 = pos[p1];

		p2 = (p1 % lens) + 1;

		if (opr[p1]==opr[p2]) {

			opr[p1] = 3 - opr[p1];

			opr[p2] = 3 - opr[p2];

			nac = nac + 1;

		} else {

			top = opr[p1];

			opr[p1] = opr[p2];

			opr[p2] = top; //@ why even have top if this is all its used for?

		}
		
		if (p2<p1) {

		 flp = -flp;

		}

	}

	spn[s] = spn[s] * flp;

	for (int k=1; k<(lens+1); k++) {

		ii = pos1[k][s];

		if (opr[k]==1) {

			stra[ii] = strb[ii];

		} else {

			stra[ii] = -1;

		}

	}

	ar3 = ar3 + double(nac);

	if ((nupd == lens)&&((!con1[0][s]))) {

		if (rand_num()<0.75) {

			spn[s] = -spn[s];

			ar4 = ar4 + 1.0;

		}

	}


}



//@----------------------------------------------------------------



void checkl(long int step, ofstream& myfile1, MTRand_open& rand_num) {

	int p, dl, l1;

	//@if ((step%500)==0){

	//@	cout << "step = " << step << " has l = " << l <<  " and has nh = " << nh << "\n\n";

	//@}

	dl = l/10 + 2;

	if (nh<(l - dl/2)) {

		return;

	}

	l1 = l + dl;

	for (int i=1; i<(l1+1); i++) {

		lstr[i] = true;

	}

	for (int j=1; j<(dl+1); j++) {

		while (true) {

			p = min(int(rand_num()*l1) + 1, l1);

			if (lstr[p]) {

				lstr[p] = false;

				break;

			} else {

				continue;

			}

		}

	}

	int k = 0;

	for (int c=1; c<(l1+1); c++) {

		if (lstr[c]) {

			k = k + 1;

			tmp1[c] = stra[k];

			tmp2[c] = strb[k];

		} else {

			tmp1[c] = 0;

			tmp2[c] = 0;

		}

	}

	l = l1;

	for (int d=1; d<(l+1); d++) {

		stra[d] = tmp1[d];

		strb[d] = tmp2[d];

	}

	pvectors();

	myfile1 << step << " increased l to " << l << "\n\n";


}



//@----------------------------------------------------------------



void errcheck() {

	int s1, s2, ix;

	for (int i=1; i<(nx+1); i++) {

		spn1[i] = spn[i];

	}
	
	for (int j=1; j<(l+1); j++) {

		s1 = stra[j];

		s2 = strb[j];

		if ((s1<-1)||(s1>nx)) {

 			cout << "Illegal s1 operator on an errcheck() j = " << j << " where s1 = " << s1 << "\n\n";

			return;

		}
		
		if ((s2<0)||(s2>nx)) {

			cout << "Illegal s2 operator on an errcheck() j = " << j << " where s2 = " << s2 << "\n\n";

			return;

		}

		if (s1>0) {

			ix = disx[ x1[s1] ][ x1[s2] ];			

			if (!lis[ spn1[s1]+1 ][ spn1[s2]+1 ][ix]) {

				cout << "Illegal Ising bond on an errcheck() j = " << j << " where Ising bond = " << lis[ spn[s1]+1 ][ spn[s2]+1 ][ix] << ", ix = " << ix << "\n\n";

				return;

			}

		} else if (s1==-1) {

			spn1[s2] = -spn1[s2];

		}

	}

	for (int k=1; k<(nx+1); k++) {

		if (spn[k]!=spn1[k]) {

			cout << "Wrong state propagion, spin index = " << k << "\n\n"; /*@ Unless he meant the french word, propagion is probably a typo for propagation*/

			return;

		}

	}


}



//@----------------------------------------------------------------



void writeconf(ofstream& myfile2) {
	
	myfile2 << "Lx   = " << nx << "\n\n";

	myfile2 << "Ht   = " << ht << "\n\n";

	myfile2 << "beta = " << beta << "\n\n";

	myfile2 << "L    = " << l << "\n\n";

	myfile2 << "----------------" << "\n\n\n";

	for (int i=1; i<(nx+1); i++) {

		myfile2 << i << " " << spn[i] << "\n";

	}

	myfile2 << "\n" << "----------------" << "\n\n\n";

	for (int j=1; j<(l+1); j++) {

		myfile2 << stra[j] << " " << strb[j] << "\n";

	}

	myfile2 << "\n" << "----------------" << "\n\n";

}



//@----------------------------------------------------------------



void measure() {

	int s1, s2, mu, nu, ni, nt, nh1, ssum, last;
	double su1, xu1, magpow2, magpow4;

	nu = 0;

	ni = 0;

	nt = 0;

	mu = 0;

	ssum = 0;

	last = 0;

	for (int i=1; i<(nx+1); i++) {

		mu = mu + spn[i];

	}

	magpow2 = pow((double(mu)/double(nx)), 2.0);

	magpow4 = pow((double(mu)/double(nx)), 4.0);

	//@ <M^2> and <M^4>

	su1 = double(pow(mu,2));

	nh1 = 0;

	for (int j=1; j<(l+1); j++) {

		s1 = stra[j];

		if (s1!=0) {

			nh1 = nh1 + 1;

			s2 = strb[j];

			if (s1==-1) {

				nt = nt + 1;

				ssum = ssum + (nh1 - last)*mu;

				last = nh1;

				spn[s2] = -spn[s2];

				mu = mu + 2*spn[s2];

			} else if (s1==s2) {

				nu = nu + 1;

			} else {

				ni = ni + 1;

			}

			su1 = su1 + double(mu*mu);

		}

	}

	ssum = ssum + (nh1 - last)*mu;

	su1 = su1/double((nh1 + 1)*nx);

	if (nh1!=0) {

		xu1 = double(pow(ssum,2))/(double(nh1*nx)*double(nh1 + 1));

	} else {

		xu1 = 0.0;

	}

	xu1 = double(beta)*(xu1 + su1/double(nh1+1));

	su = su + su1;

	xu = xu + xu1;

	avnu = avnu + double(nu);

	avni = avni + double(ni);

	avnt = avnt + double(nt);

	avn1 = avn1 + double(nh1);

	avn2 = avn2 + double(pow(nh1,2));

	nmsr = nmsr + 1;


	av_magpow2 = av_magpow2 + magpow2;

	av_magpow4 = av_magpow4 + magpow4;

	//@ <M^2> and <M^4> before dividing by nmsr

}



//@----------------------------------------------------------------



void results(ofstream& myfile3, ofstream& myfile4, ofstream& myfile6, ofstream& myfile7, ofstream& myfile8, ofstream& myfile9) {
	//~ ************ CHANGING


	double c, e;

	xu = xu/double(nmsr);

	su = su/double(nmsr);

	avnu = avnu/double(nmsr);

	avni = avni/double(nmsr);

	avnt = avnt/double(nmsr);

	avn1 = avn1/double(nmsr);

	avn2 = avn2/double(nmsr);

	c = (avn2 - pow(avn1,2) - avn1)/double(nx);

	e = eshft - (avn1 - avnu)/(double(beta)*double(nx));

	avnu = avnu/(ht*double(beta)*double(nx));

	avni = eshft/double(nx) - avni/(double(beta)*double(nx));

	avnt = -avnt/(double(beta)*double(nx));

	av_magpow2 = av_magpow2/double(nmsr);

	av_magpow4 = av_magpow4/double(nmsr);

	//@ <M^2> and <M^4>

	long double Binder_cumulant = ((long double) 1) - ( ((long double)av_magpow4) / (((long double) 3) * ((long double)(pow(av_magpow2, 2)) )) );

	//@ Binder_cumulant

	myfile3 << su << " " << xu << "\n";

	myfile4 << e << " " << c << " " << avni << " " << avnt << " " << avnu << "\n";

	if (temp == final_temp){

		myfile6 << avn1;

		myfile7 << av_magpow2;

		myfile8 << e;

		myfile9 << Binder_cumulant;

	} else {

		myfile6 << avn1 << "\n";

		myfile7 << av_magpow2 << "\n";

		myfile8 << e << "\n";

		myfile9 << Binder_cumulant << "\n";

	}

	zerodat(); 

}



//@----------------------------------------------------------------



void zerodat() {
	
	su = 0.0;

	xu = 0.0;

	avnu = 0.0;

	avni = 0.0;

	avnt = 0.0;

	avn1 = 0.0;

	avn2 = 0.0;

	nmsr = 0;


	av_magpow2 = 0.0;

	av_magpow4 = 0.0;


}



//@----------------------------------------------------------------



void writeacc(ofstream& myfile5) {

	ar1 = ar1/double(msteps);

	ar2 = ar2/double(msteps);

	ar3 = ar3/double(msteps);

	ar4 = ar4/double(msteps);

	myfile5 << "Diagonal acceptance 1     : " << ar1 << "\n"; 

	myfile5 << "Diagonal acceptance 2     : " << ar2 << "\n"; 

	myfile5 << "Off-Diagonal substitions  : " << ar3 << "\n"; 

	myfile5 << "Spin flips / site         : " << ar4 << "\n"; 

	myfile5 << "Max lsub                  : " << mlls << "\n\n";
 
}



//@----------------------------------------------------------------



//void calcenrcool(int step, ofstream& myfile8) {

	//int nh_c = 0;

	//for (int i=1; i<(l+1); i++) {

		//if (stra[i]!=0) {

			//nh_c = nh_c + 1;

		//}

	//}

	//myfile8 << step << " " << nh_c << "\n";


//}



//@----------------------------------------------------------------
