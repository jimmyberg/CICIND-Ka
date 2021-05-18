#include "AerodynamicDampingVortex.h"
#include "Interpolation.h"
#include <fstream>

void AerodynamicDampingVortex::loadKaMean(const char* filePath){
	// File is a typical multi collum data set. First row holds titles and
	// turbulance scales for each collumn. First collumn holds relative wind
	// critical speed v/v_cr. Then each row contains the aerodynamic damping
	// parameter.
	std::ifstream iFile;
	iFile.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

	iFile.open(filePath);
	// For basic exception safety guarantee. If something throws, we make sure
	// everything is valid.
	try{
		char charBuffer[16];
		numberOfTurbulenceScales = 7;
		numberOfWindspeedScales = 122;

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
		iFile.getline(charBuffer, 256, ',');
		for (unsigned int turbulenceIndex = 0; turbulenceIndex < numberOfTurbulenceScales; ++turbulenceIndex){
			iFile.getline(charBuffer, 256, ',');
			ka_mean_turbulence[turbulenceIndex] = atof(charBuffer);
		}
		for (unsigned int speedIndex = 0; speedIndex < numberOfWindspeedScales; ++speedIndex){
			// iFile.ignore(256, '\n');
			iFile.getline(charBuffer, 256, ',');
			ka_mean_referenceSpeed[speedIndex] = atof(charBuffer);
			for (unsigned int turbulenceIndex = 0; turbulenceIndex < numberOfTurbulenceScales; ++turbulenceIndex){
				const unsigned int offSet = turbulenceIndex*numberOfWindspeedScales;
				float* zArray = &ka_mean_damping[offSet];
				iFile.getline(charBuffer, 256, ',');
				zArray[speedIndex] = atof(charBuffer);
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
