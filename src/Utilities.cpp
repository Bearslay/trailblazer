#include "Utilities.hpp"

template <typename Type> Type btils::set(Type &lhs, const Type &rhs) {
    const Type output = lhs;
    lhs = rhs;
    return output;
}
template <typename Type> Type btils::adj(Type &lhs, const Type &rhs) {
    const Type output = lhs;
    lhs += rhs;
    return output;
}

template <typename InArithType, typename OutArithType> OutArithType btils::map(const InArithType &input, const InArithType &inMin, const InArithType &inMax, const OutArithType &outMin, const OutArithType &outMax) {
    static_assert(std::is_arithmetic< InArithType>::value, "InArithType must be an arithmetic type");
    static_assert(std::is_arithmetic<OutArithType>::value, "OutArithType must be an arithmetic type");
    return outMin + ((OutArithType)input - (OutArithType)inMin) * (outMax - outMin) / ((OutArithType)inMax - (OutArithType)inMin);
}
template <typename ArithType> ArithType btils::map(const ArithType &input, const ArithType &inMin, const ArithType &inMax, const ArithType &outMin, const ArithType &outMax) {return map<ArithType, ArithType>(input, inMin, inMax, outMin, outMax);}

template <typename ArithType> ArithType btils::normalize(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
    static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
    const ArithType width = rangeMax - rangeMin;
    const ArithType value = input - rangeMin;
    return (value - (std::floor(value / width) * width)) + rangeMin;
}
template <typename ArithType> ArithType btils::normalize(const ArithType &input, const ArithType &max) {
    static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
    return input - std::floor(input / max) * max;
}

template <typename ArithType> ArithType btils::clamp(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
    static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
    return input < rangeMin ? rangeMin : (input > rangeMax ? rangeMax : input);
}
template <typename ArithType> ArithType btils::clamp(const ArithType &input, const ArithType &max) {
    static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
    return input < 0 ? 0 : (input > max ? max : input);
}

double btils::degToRad(const double &angle) {return angle * U_PI_180;}
double btils::radToDeg(const double &angle) {return angle * U_180_PI;}
double btils::convertAngle(const double &angle, const bool &toRadians) {return toRadians ? degToRad(angle) : radToDeg(angle);}
double btils::normalizeDegree(const double &angle) {return normalize<double>(angle, 360.0);}
double btils::normalizeRadian(const double &angle) {return normalize<double>(angle, C_2PI);}
double btils::normalizeAngle(const double &angle, const bool &useRadians) {return useRadians ? normalizeRadian(angle) : normalizeDegree(angle);}
