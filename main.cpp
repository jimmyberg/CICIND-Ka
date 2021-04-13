#include <iostream>
#include "AerodynamicDampingVortex.h"

using namespace std;

int main(int argc, char** argv){
	AerodynamicDampingVortex K_a0;
	K_a0.loadKaMean("Ka_mean.csv");

	cout << "Aerodynamic damping at turblance = 0.11 and windspeed ratio 1.2 (v/v_cr) = " << K_a0.getAerodynamicDampingParameter(0.11, 1.2) << endl;

	return 0;
}
