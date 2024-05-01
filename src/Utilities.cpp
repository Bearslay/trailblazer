#include "Utilities.hpp"

double btils::degToRad(const double &angle) {return angle * U_PI_180;}
double btils::radToDeg(const double &angle) {return angle * U_180_PI;}
double btils::convertAngle(const double &angle, const bool &toRadians) {return toRadians ? degToRad(angle) : radToDeg(angle);}
double btils::normalizeDegree(const double &angle) {return normalize<double>(angle, 360.0);}
double btils::normalizeRadian(const double &angle) {return normalize<double>(angle, C_2PI);}
double btils::normalizeAngle(const double &angle, const bool &useRadians) {return useRadians ? normalizeRadian(angle) : normalizeDegree(angle);}
