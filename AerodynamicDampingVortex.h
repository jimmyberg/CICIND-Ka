#ifndef _AERODYNAMICDAMPINGVORTEX_H_
#define _AERODYNAMICDAMPINGVORTEX_H_

class AerodynamicDampingVortex{
public:
	/**
	 * @brief      Loads a ka mean from file.
	 *
	 * @param[in]  filePath  The file path to ka mean file
	 */
	void loadKaMean(const char* filePath);
	/**
	 * @brief      Gets the aerodynamic damping parameter.
	 *
	 * @par Reference
	 *
	 *             CICIND commentaries Figure 3.3.4 given by file
	 *
	 * @param[in]  turbulenceIntensity     The turbulence intensity
	 * @param[in]  windspeedRatioCritical  The wind speed ratio critical
	 *
	 * @return     The aerodynamic damping parameter.
	 */
	float getAerodynamicDampingParameter(float turbulenceIntensity, float windspeedRatioCritical) const;
	private:
	float* ka_mean_turbulence = nullptr; ///< array to turbulence values
	float* ka_mean_referenceSpeed = nullptr; ///< Expressed as wind speed / critical wind speed. v_cr = f*d/St
	float* ka_mean_damping = nullptr; ///< array to damping values
	unsigned int numberOfTurbulenceScales; ///< array size of turbulence
	unsigned int numberOfWindspeedScales; ///< array size of wind speed
};

#endif //_AERODYNAMICDAMPINGVORTEX_H_
