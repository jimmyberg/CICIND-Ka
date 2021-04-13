#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#include <cstddef>

/**
 * @brief      This class describes a lin log interpolate.
 */
class LinLogInterpolate{
public:
	LinLogInterpolate(float x1, float x2, float y1, float y2);
	/**
	 * @brief      Interpolate by limit for values outside x1 and x2
	 *
	 * @param[in]  x     interpolate at value
	 *
	 * @return     Value interpolated
	 */
	float interpolateFlat(float x) const;
	/**
	 * @brief      Interpolate/extrapolate value
	 *
	 * @param[in]  x     interpolate at value
	 *
	 * @return     Value interpolated
	 */
	float interpolate(float x) const;
private:
	float x1;
	float x2;
	float y1;
	float y2;
	const float lnX2X1;
};

/**
 * @brief      This class describes a lin interpolate.
 */
class LinInterpolate{
public:
	LinInterpolate(float x1, float x2, float y1, float y2);
	/**
	 * @brief      Interpolate by limit for values outside x1 and x2
	 *
	 * @param[in]  x     interpolate at value
	 *
	 * @return     Value interpolated
	 */
	float interpolateFlat(float x) const;
	/**
	 * @brief      Interpolate/extrapolate value
	 *
	 * @param[in]  x     interpolate at value
	 *
	 * @return     Value interpolated
	 */
	float interpolate(float x) const;
private:
	float x1;
	float x2;
	float y1;
	float y2;
	const float X2X1; // 1/(x_2-x_1)
};

/**
 * @brief      This class is capabile of cubic 3d interpolation.
 */
class Cubic3DInterpolation{
public:
	/**
	 * @brief      Constructs a new instance.
	 *
	 * @param      xArray       The x array
	 * @param      yArray       The y array
	 * @param      resultArray  The result array constructed using a 1D array encoded by coordinats: x*sizeX + y
	 * @param[in]  sizeX        The size x
	 * @param[in]  sizeY        The size y
	 */
	Cubic3DInterpolation(
		float* xArray,
		float* yArray,
		float* resultArray,
		unsigned int sizeX,
		unsigned int sizeY
	):
		xArray(xArray),
		yArray(yArray),
		resultArray(resultArray),
		sizeX(sizeX),
		sizeY(sizeY)
	{}
	/**
	 * @brief      Calculate interpolated value at x and y
	 *
	 * @param[in]  x     x value
	 * @param[in]  y     y value
	 *
	 * @return     interpolated value
	 */
	float evalQubic(float x, float y);
private:
	float evalQubicYzero(float x, float y);
	float evalQubicYRoof(float x, float y);
	size_t findZero(float target, float array[], unsigned int size);
	float getResult(unsigned int xIndex, unsigned int yIndex);
	float getxDerResult(unsigned int xIndex, unsigned int yIndex);
	float getyDerResult(unsigned int xIndex, unsigned int yIndex);
	float cubicInterpolateYgrid(float x, unsigned int zeroXIndex, unsigned int yIndex);
	/**
	 * @brief      cubic interpolation by using two coordinates with their value
	 *             and derivative.
	 *
	 * @param[in]  x        the intermediate point to interpolate
	 * @param      xPoints  The point x coordinates
	 * @param      values   The values at the x coordinates followed by two
	 *                      derivatives at those points.
	 *
	 * @return     Interpolated value
	 */
	float cubicInterpolateByVal(float x, float xPoints[2], float values[4]);
	float* xArray;
	float* yArray;
	float* resultArray;
	unsigned int sizeX;
	unsigned int sizeY;
};

extern "C"{

/**
 * @brief      interpolation  function to be used by FORTRAN
 *
 * @param[in]  x            the desired x coordinate
 * @param[in]  y            the desired y coordinate
 * @param      xArray       The array of the x points
 * @param      yArray       The array of the y points
 * @param      resultArray  The "z values" for every x and y coordinates
 * @param[in]  sizeX        The size of xArray
 * @param[in]  sizeY        The size of yArray
 *
 * @return     value at x and y
 */
float QD2VL(
	const float* x,
	const float* y,
	float* xArray,
	float* yArray,
	float* resultArray,
	const unsigned int* sizeX,
	const unsigned int* sizeY
);
}

#endif //_INTERPOLATION_H_
