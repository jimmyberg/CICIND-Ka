#include "Interpolation.h"
#include "Matrix.h"

#include <iostream>
#include <cmath>
#include <stdexcept>
using namespace std;

LinLogInterpolate::LinLogInterpolate(float x1, float x2, float y1, float y2):
	x1(x1), x2(x2), y1(y1), y2(y2), lnX2X1(1.0/log(x2/x1)){}

float LinLogInterpolate::interpolateFlat(float x) const{
	if(x <= x1)
		return y1;
	else if(x < x2)
		// Using "linear-logarithmic" interpolation such that ploting on
		// semilogx shows a straight line.
		return log(x/x1) * (y2-y1) * lnX2X1 + y1;
	else
		return y2;
}

float LinLogInterpolate::interpolate(float x) const{
	return log(x/x1) * (y2-y1) * lnX2X1 + y1;
}

LinInterpolate::LinInterpolate(float x1, float x2, float y1, float y2):
	x1(x1), x2(x2), y1(y1), y2(y2), X2X1(1.0/(x2-x1)){}


float LinInterpolate::interpolateFlat(float x) const{
	if(x <= x1)
		return y1;
	else if(x < x2)
		return (x-x1) * (y2-y1) * X2X1 + y1;
	else
		return y2;
}

float LinInterpolate::interpolate(float x) const{
	return (x-x1) * (y2-y1) * X2X1 + y1;
}

float Cubic3DInterpolation::evalQubic(float x, float y){
	// The goal is to find the closest known points xn and yn to x and y.
	// From there the adjacent points can be used to quadratically
	// interpolate.
	x = max(xArray[0], x);
	y = max(yArray[0], y);
	x = min(xArray[sizeX-1], x);
	y = min(yArray[sizeY-1], y);
	const size_t zeroYIndex = findZero(y, yArray, sizeY);
	// Check if extrapolation on the y edges is needed. If so, go to separate
	// handlers.
	if(zeroYIndex == 0)
		return evalQubicYzero(x, y);
	else if(zeroYIndex == sizeY-2)
		return evalQubicYRoof(x, y);
	const size_t zeroXIndex = findZero(x, xArray, sizeX);
	// Create four 2D interpolated points on the yz plane at location x that
	// will in turn function as interpolation values for y.
	float interpolationX[4]{
		cubicInterpolateYgrid(x, zeroXIndex, zeroYIndex-1),
		cubicInterpolateYgrid(x, zeroXIndex, zeroYIndex+0),
		cubicInterpolateYgrid(x, zeroXIndex, zeroYIndex+1),
		cubicInterpolateYgrid(x, zeroXIndex, zeroYIndex+2)
	};

	float values[4]{
		interpolationX[1],
		interpolationX[2],
		(interpolationX[2] - interpolationX[0]) / (yArray[zeroYIndex+1] - yArray[zeroYIndex-1]),
		(interpolationX[3] - interpolationX[1]) / (yArray[zeroYIndex+2] - yArray[zeroYIndex])
	};

	return cubicInterpolateByVal(y, &yArray[zeroYIndex], values);
}

float Cubic3DInterpolation::evalQubicYzero(float x, float y){
	// The goal is to find the closest known points xn and yn to x and y.
	// From there the adjacent points can be used to quadratically
	// interpolate.

	const size_t zeroXIndex = findZero(x, xArray, sizeX);
	// Create four 2d interpolated points on the yz plane at location x that
	// will in turn function as interpolation values for y.
	float interpolationX[3]{
		cubicInterpolateYgrid(x, zeroXIndex, 0),
		cubicInterpolateYgrid(x, zeroXIndex, 1),
		cubicInterpolateYgrid(x, zeroXIndex, 2)
	};

	float values[4]{
		interpolationX[0],
		interpolationX[1],
		(interpolationX[1] - interpolationX[0]) / (yArray[1] - yArray[0]),
		(interpolationX[2] - interpolationX[0]) / (yArray[2] - yArray[0])
	};
	return cubicInterpolateByVal(y, &yArray[0], values);
}
float Cubic3DInterpolation::evalQubicYRoof(float x, float y){
	// The goal is to find the closest known points xn and yn to x and y.
	// From there the adjacent points can be used to quadratically
	// interpolate.

	const size_t zeroXIndex = findZero(x, xArray, sizeX);
	// Create four 2D interpolated points on the yz plane at location x that
	// will in turn function as interpolation values for y.
	float interpolationX[3]{
		cubicInterpolateYgrid(x, zeroXIndex, sizeY-3),
		cubicInterpolateYgrid(x, zeroXIndex, sizeY-2),
		cubicInterpolateYgrid(x, zeroXIndex, sizeY-1)
	};

	float values[4]{
		interpolationX[1],
		interpolationX[2],
		(interpolationX[2] - interpolationX[0]) / (yArray[sizeY-1] - yArray[sizeY-3]),
		(interpolationX[2] - interpolationX[1]) / (yArray[sizeY-1] - yArray[sizeY-2])
	};
	return cubicInterpolateByVal(y, &yArray[sizeY-2], values);
}

size_t Cubic3DInterpolation::findZero(float target, float array[], unsigned int size){
	// The goal is to find lower point to the known point xn from the target.
	if(target > array[size-1] || target < array[0])
		throw std::runtime_error("Attempted bi-cubic interpolation outside domain.");
	// Find point above target point
	for (unsigned int currentIndex = size-2; currentIndex+1 != 0; --currentIndex){
		if(target >= array[currentIndex]){
			return currentIndex;
		}
	}
	// Should not reach here.
	return 0;
}

float Cubic3DInterpolation::getResult(unsigned int xIndex, unsigned int yIndex){
	if(xIndex > sizeX || yIndex > sizeY)
		throw std::runtime_error("Tried to access value outside domain.");
	return resultArray[xIndex*sizeY + yIndex];
}

float Cubic3DInterpolation::getxDerResult(unsigned int xIndex, unsigned int yIndex){
	// The derivative is the only functionality that needs to be able to use
	// data outside of domain. Therefor linear extrapolation will be used to
	// predict the derivative outside the domain.
	if(xIndex == 0)
		return (getResult(xIndex+1, yIndex) - getResult(xIndex, yIndex))   / (xArray[xIndex+1] - xArray[xIndex]);
	else if(xIndex == sizeX-1)
		return (getResult(xIndex, yIndex)   - getResult(xIndex-1, yIndex)) / (xArray[xIndex]   - xArray[xIndex-1]);
	else
		return (getResult(xIndex+1, yIndex) - getResult(xIndex-1, yIndex)) / (xArray[xIndex+1] - xArray[xIndex-1]);
}

float Cubic3DInterpolation::getyDerResult(unsigned int xIndex, unsigned int yIndex){
	if(yIndex == 0)
		return (getResult(xIndex, yIndex+1) - getResult(xIndex, yIndex))   / (yArray[yIndex+1] - yArray[yIndex]);
	else if(yIndex == sizeY-1)
		return (getResult(xIndex, yIndex)   - getResult(xIndex, yIndex-1)) / (yArray[yIndex]   - yArray[yIndex-1]);
	else
		return (getResult(xIndex, yIndex+1) - getResult(xIndex, yIndex-1)) / (yArray[yIndex+1] - yArray[yIndex-1]);
}

float Cubic3DInterpolation::cubicInterpolateYgrid (float x, unsigned int zeroXIndex, unsigned int yIndex) {
	float xPoints[2]{
		xArray[zeroXIndex],
		xArray[zeroXIndex+1]
	};
	float values[4]{
		getResult(zeroXIndex, yIndex),
		getResult(zeroXIndex+1, yIndex),
		getxDerResult(zeroXIndex, yIndex),
		getxDerResult(zeroXIndex+1, yIndex)
	};
	return cubicInterpolateByVal(x, xPoints, values);
}

float Cubic3DInterpolation::cubicInterpolateByVal(float x, float xPoints[2], float values[4]){
	float x0Sq = xPoints[0]*xPoints[0];
	float x1Sq = xPoints[1]*xPoints[1];
	float x0Cu = x0Sq*xPoints[0];
	float x1Cu = x1Sq*xPoints[1];
	Matrix solver(4);
	solver.A[0][0] = x0Cu;
	solver.A[1][0] = x1Cu;
	solver.A[2][0] = 3*x0Sq;
	solver.A[3][0] = 3*x1Sq;

	solver.A[0][1] = x0Sq;
	solver.A[1][1] = x1Sq;
	solver.A[2][1] = 2*xPoints[0];
	solver.A[3][1] = 2*xPoints[1];

	solver.A[0][2] = xPoints[0];
	solver.A[1][2] = xPoints[1];
	solver.A[2][2] = 1;
	solver.A[3][2] = 1;

	solver.A[0][3] = 1;
	solver.A[1][3] = 1;
	solver.A[2][3] = 0;
	solver.A[3][3] = 0;

	solver.B[0] = values[0];
	solver.B[1] = values[1];
	solver.B[2] = values[2];
	solver.B[3] = values[3];
	solver.autoSolve();

	return solver.B[3] + x*(solver.B[2] + x*(solver.B[1] + x*(solver.B[0])));
}

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
){
	Cubic3DInterpolation solver(xArray, yArray, resultArray, *sizeX, *sizeY);
	return solver.evalQubic(*x, *y);
}
}
