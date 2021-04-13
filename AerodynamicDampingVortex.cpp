#include "AerodynamicDampingVortex.h"
#include "Interpolation.h"
#include <fstream>

void AerodynamicDampingVortex::loadKaMean(const char* filePath){
	// Function expects first four lines to be reserved for comments. Then the
	// first line of data gives the number of measured turbulences scales
	// followed by the number of mean wind speeds measured. The data consist of
	// blocks, each block contains a number representing the turbulence
	// intensity. This is then followed by samples for that turbulence intensity
	// of the wind speed followed by the damping parameter separated by a comma.
	// <turbulenceVal>
	// <speed>,<damping>
	// <speed>,<damping>
	// <speed>,<damping>
	// <speed>,<damping>
	// ...
	std::ifstream iFile;
	iFile.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

	iFile.open(filePath);
	iFile.ignore(256, '\n');
	iFile.ignore(256, '\n');
	iFile.ignore(256, '\n');
	iFile.ignore(256, '\n');
	// For basic exception safety guarantee. If something throws, we make sure
	// everything is valid.
	try{
		char charBuffer[16];
		iFile.getline(charBuffer, 256, ',');
		numberOfTurbulenceScales = atoi(charBuffer);
		iFile.getline(charBuffer, 256, '\n');
		numberOfWindspeedScales = atoi(charBuffer);
		// Allocate data space
		if(ka_mean_turbulence != nullptr)
			delete [] ka_mean_turbulence;
		ka_mean_turbulence     = new float[numberOfTurbulenceScales];
		if(ka_mean_referenceSpeed != nullptr)
			delete [] ka_mean_referenceSpeed;
		ka_mean_referenceSpeed = new float[numberOfWindspeedScales];
		if(ka_mean_damping != nullptr)
			delete [] ka_mean_damping;
		ka_mean_damping        = new float[numberOfTurbulenceScales * numberOfWindspeedScales];
		// Load data values in data space
		for (unsigned int turbulenceIndex = 0; turbulenceIndex < numberOfTurbulenceScales; ++turbulenceIndex){
			iFile.getline(charBuffer, 256, ',');
			ka_mean_turbulence[turbulenceIndex] = atof(charBuffer);
			iFile.ignore(256, '\n');
			const unsigned int offSet = turbulenceIndex*numberOfWindspeedScales;
			float* zArray = &ka_mean_damping[offSet];
			for (unsigned int speedIndex = 0; speedIndex < numberOfWindspeedScales; ++speedIndex){
				iFile.getline(charBuffer, 256, ',');
				ka_mean_referenceSpeed[speedIndex] = atof(charBuffer);
				iFile >> zArray[speedIndex];
			}
		}
	}
	catch(...){
		// Reset everything
		if(ka_mean_turbulence != nullptr)
			delete [] ka_mean_turbulence;
		if(ka_mean_referenceSpeed != nullptr)
			delete [] ka_mean_referenceSpeed;
		if(ka_mean_damping != nullptr)
			delete [] ka_mean_damping;
		numberOfTurbulenceScales = 0;
		numberOfWindspeedScales = 0;
		// Re throw
		throw;
	}
}

float AerodynamicDampingVortex::getAerodynamicDampingParameter(float turbulenceIntensity, float windspeedRatioCritical) const{
	Cubic3DInterpolation interpolator(
		ka_mean_turbulence,
		ka_mean_referenceSpeed,
		ka_mean_damping,
		numberOfTurbulenceScales,
		numberOfWindspeedScales);
	return interpolator.evalQubic(turbulenceIntensity, windspeedRatioCritical);
}
