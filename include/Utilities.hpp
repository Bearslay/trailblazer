#ifndef UTILITIES
#define UTILITIES

#include <cmath>
#include <vector>
#include <type_traits>
#include <string>

/* pi/8 rad or 22.5 deg */
#define C_PI_8     0.39269908169872415481
/* pi/6 rad or 30 deg */
#define C_PI_6     0.52359877559829887308
/* pi/4 rad or 45 deg */
#define C_PI_4     0.78539816339744830961
/* pi/3 rad or 60 deg */
#define C_PI_3     1.04719755119659774615
/* 3pi/8 rad or 67.5 deg */
#define C_3PI_8    1.17809724509617246442
/* pi/2 rad or 90 deg */
#define C_PI_2     1.57079632679489661923
/* 5pi/8 rad or 112.5 deg */
#define C_5PI_8    1.96349540849362077404
/* 2pi/3 rad or 120 deg */
#define C_2PI_3    2.09439510239319549231
/* 3pi/4 rad or 135 deg */
#define C_3PI_4    2.35619449019234492885
/* 5pi/6 rad or 150 deg */
#define C_5PI_6    2.61799387799149436539
/* 7pi/8 rad or 157.5 deg */
#define C_7PI_8    2.74889357189106908365
/* pi rad or 180 deg */
#define C_PI       3.14159265358979323846
/* 9pi/8 rad or 202.5 deg */
#define C_9PI_8    3.53429173528851739327
/* 7pi/6 rad or 210 deg */
#define C_7PI_6    3.66519142918809211154
/* 5pi/4 rad or 225 deg */
#define C_5PI_4    3.92699081698724154808
/* 4pi/3 rad or 240 deg */
#define C_4PI_3    4.18879020478639098462
/* 11pi/8 rad or 247.5 deg */
#define C_11PI_8   4.31968989868596570289
/* 3pi/2 rad or 270 deg */
#define C_3PI_2    4.71238898038468985769
/* 13pi/8 rad or 292.5 deg */
#define C_13PI_8   5.10508806208341401250
/* 5pi/3 rad or 300 deg */
#define C_5PI_3    5.23598775598298873077
/* 7pi/4 rad or 315 deg */
#define C_7PI_4    5.49778714378213816731
/* 11pi/6 rad or 330 deg */
#define C_11PI_6   5.75958653158128760385
/* 15pi/8 rad or 337.5 deg */
#define C_15PI_8   5.89048622548086232212
/* 2pi rad or 360 deg */
#define C_2PI      6.28318530717958647693

/* The value of sqrt(3)/2 */
#define C_SQRT3_2  0.86602540378443864676
/* The value of sqrt(2)/2 */
#define C_SQRT2_2  0.70710678118654752440

/* The value of cos(pi/8) or sqrt(2+sqrt(2))/2 */
#define C_MULT_1_8 0.92387953251128675613
/* The value of cos(pi/6) or sqrt(3)/2 */
#define C_MULT_1_6 0.86602540378443864676
/* The value of cos(pi/4) or sqrt(2)/2 */
#define C_MULT_1_4 0.70710678118654752440
/* The value of cos(pi/3) or 1/2 */
#define C_MULT_1_3 0.50000000000000000000
/* The value of cos(3pi/8) or sqrt(2-sqrt(2))/2 */
#define C_MULT_3_8 0.38268343236508977173

/* The value of sqrt(2) */
#define C_SQRT2    1.41421356237309504880
/* The value of sqrt(3) */
#define C_SQRT3    1.73205080756887729353

/* The conversion factor from degrees to radians (pi/180) */
#define U_PI_180   0.01745329251994329577
/* The conversion factor from radians to degrees (180/pi) */
#define U_180_PI   57.2957795130823208770

namespace btils {
    /** Acts as an assignment operator, but while returning the previous value of the thing being modified
     * @tparam Type A something that has a defined assignment operator (primitive type or otherwise)
     * @param lhs The left-hand side of an assignment operator (value will be altered)
     * @param rhs The right-hand side of an assignment operator (value will not be altered)
     * @returns The original value of lhs    */
    template <typename Type> Type set(Type &lhs, const Type &rhs) {
        const Type output = lhs;
        lhs = rhs;
        return output;
    }
    /** Acts as an addition assignment operator, but while returning the previous value of the thing being modified
     * @tparam Type A something that has a defined addition assignment operator (primitive type or otherwise)
     * @param lhs The left-hand side of an addition assignment operator (value will be altered)
     * @param rhs The right-hand side of an addition assignment operator (value will not be altered)
     * @returns The original value of lhs    */
    template <typename Type> Type adj(Type &lhs, const Type &rhs) {
        const Type output = lhs;
        lhs += rhs;
        return output;
    }

    /** Maps a value within a range onto a different range
     * @tparam InArithType An arithmetic data type for the input & starting range
     * @tparam OutArithType An arithmetic data type for the output & ending range
     * @param input Value to be mapped (of InArithType type)
     * @param inMin Minimum value for the starting range (of InArithType type)
     * @param inMax Maximum value for the starting range (of InArithType type)
     * @param outMin Minimum value for the ending range (of OutArithType type)
     * @param outMax Maximum value for the ending range (of OutArithType type)
     * @returns A value within the ending range matching the input value's position within the starting range (of OutArithType type)    */
    template <typename InArithType, typename OutArithType> OutArithType map(const InArithType &input, const InArithType &inMin, const InArithType &inMax, const OutArithType &outMin, const OutArithType &outMax) {
        static_assert(std::is_arithmetic< InArithType>::value, "InArithType must be an arithmetic type");
        static_assert(std::is_arithmetic<OutArithType>::value, "OutArithType must be an arithmetic type");
        return outMin + (OutArithType)(input - inMin) * (outMax - outMin) / (OutArithType)(inMax - inMin);
    }
    /** Maps a value within a range onto a different range
     * @tparam ArithType An arithmetic data type for the input & starting range
     * @param input Value to be mapped
     * @param inMin Minimum value for the starting range
     * @param inMax Maximum value for the starting range
     * @param outMin Minimum value for the ending range
     * @param outMax Maximum value for the ending range
     * @returns A value within the ending range matching the input value's position within the starting range    */
    template <typename ArithType> ArithType map(const ArithType &input, const ArithType &inMin, const ArithType &inMax, const ArithType &outMin, const ArithType &outMax) {return map<ArithType, ArithType>(input, inMin, inMax, outMin, outMax);}

    /** Normalize a value to a range (assuming the range is a loop)
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to normalize
     * @param rangeMin The smallest value making up the range (inclusive)
     * @param rangeMax The largest value making up the range (exclusive)
     * @returns A value normalized in the given range    */
    template <typename ArithType> ArithType normalize(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        const ArithType width = rangeMax - rangeMin;
        const ArithType value = input - rangeMin;
        return (value - (std::floor(value / width) * width)) + rangeMin;
    }
    /** Normalize a value to a range with a minimum value of 0 (assuming the range is a loop)
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to normalize
     * @param max The largest value making up the range (exclusive)
     * @returns A value normalized in the given range    */
    template <typename ArithType> ArithType normalize(const ArithType &input, const ArithType &max) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input - std::floor(input / max) * max;
    }

    /** Clamp a value to a range
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to clamp
     * @param rangeMin The smallest value making up the range (inclusive)
     * @param rangeMax The largest value making up the range (inclusive)
     * @returns A value clammped to the given range    */
    template <typename ArithType> ArithType clamp(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input < rangeMin ? rangeMin : (input > rangeMax ? rangeMax : input);
    }
    /** Clamp a value to a range with a minimum value of 0
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to clamp
     * @param max The largest value making up the range (inclusive)
     * @returns A value clammped to the given range    */
    template <typename ArithType> ArithType clamp(const ArithType &input, const ArithType &max) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input < 0 ? 0 : (input > max ? max : input);
    }

    /** Convert an angle from degrees to radians
     * @param angle An angle in degrees
     * @returns An angle in radians    */
    double degToRad(const double &angle);
    /** Convert an angle from radians to degrees
     * @param angle An angle in radians
     * @returns An angle in degrees    */
    double radToDeg(const double &angle);
    /** Convert an angle from either degrees to radians or radians to degrees
     * @param angle An angle in either radians or degrees
     * @param toRadians Whether to convert to radians or degrees
     * @returns An angle measured with the specified unit    */
    double convertAngle(const double &angle, const bool &toRadians = true);
    /** Normalize an angle (in degrees) to be in the range of [0, 360)
     * @param angle An angle in degrees
     * @returns An angle in degrees, but in the range of [0, 360)    */
    double normalizeDegree(const double &angle);
    /** Normalize an angle (in radians) to be in the range of [0, 2 * pi)
     * @param angle An angle in radians
     * @returns An angle in radians, but in the range of [0, 2 * pi)    */
    double normalizeRadian(const double &angle);
    /** Normalize an angle to be in the range of either [0, 2 * pi) or [0, 360)
     * @param angle An angle in either radians or degrees
     * @param useRadians Whether to normalize to [0, 2 * pi) for radians or [0, 360) for degrees
     * @returns An angle, but in the range of either [0, 2 * pi) or [0, 360)    */
    double normalizeAngle(const double &angle, const bool &useRadians = true);

    /** Get the index of a target element within a sorted list
     * @tparam Type Any datatype/class; must have relational operators available
     * @param list The sorted list to search through
     * @param target The element being targetted
     * @param lowerBounds The lowest index within the list to search from (can be used to search a particular section of a larger list)
     * @param upperBounds the highest index within the list to search from (can be used to search a particular section of a larger list)
     * @returns The index of the target element if found, or a -1 if the target is not present in the list    */
    template <typename Type> long long binarySearch(const std::vector<Type> &list, const Type &target, const unsigned long int &lowerBounds = 0, const unsigned long int &upperBounds = __UINT64_MAX__) {
        if (list.size() == 0) {return -1;}

        unsigned long int lb = lowerBounds > list.size() ? list.size() - 1 : lowerBounds;
        unsigned long int ub = upperBounds > list.size() ? list.size() - 1 : upperBounds;
        while (lb <= ub) {
            unsigned long int i = lb + (ub - lb) / 2;
            if (list.at(i) == target) {return i;}
            if (list.at(i) < target) {lb = i + 1;}
            else {ub = i - 1;}
        }
        return -1;
    }

    /** Convert an arithmetic data type to an std::string with some extra formatting for floating-point types
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @returns An std::string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions    */
    template <typename ArithType> std::string toString(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");

        // std::to_string() is acceptable for integral types, floating point types are where the formatting is needed
        if (std::is_integral<ArithType>::value) {return std::to_string(input);}

        std::string output = std::to_string(input);
        // Remove the last character if it is either a zero or a decimal
        while (output.back() == '0') {
            output.pop_back();
            // Break seperately at the decimal so that zeros in positive places aren't removed
            if (output.back() == '.') {
                output.pop_back();
                break;
            }
        }
        return output;
    }
    /** Convert an arithmetic data type to an std::string with some extra formatting
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negative sign; trailing zeros and decimals are omitted from floating-point conversions    */
    template <typename ArithType> std::string toString(const ArithType &input, const bool alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return (alwaysIncludeSign && input >= 0 ? "+" : "") + btils::toString<ArithType>(input);
    }

    /** Convert an arithmetic data type to an std::string with extra leading/trailing zeros if specified
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param leftDigits The minimum amount of digits that will be to the left of the decimal; metric reached by adding leading zeros
     * @param rightDigits The minimum amount of digits that will be to the right of the decimal; metric reached by adding trailing zeros
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not (sign counts as a 'left digit')
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negatie sign; leading and trailing zeros are also included in order to meet certain parameters    */
    template <typename ArithType> std::string tstr_AddZeros(const ArithType &input, const unsigned long int &leftDigits, const unsigned long int &rightDigits, const bool &alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string output = btils::toString<ArithType>(input, alwaysIncludeSign);
        const unsigned long int decimalPos = output.find('.');
        const unsigned long int before = decimalPos == std::string::npos ? output.length() : decimalPos, after = decimalPos == std::string::npos ? 0 : output.length() - before - 1;
        const bool insertPos = alwaysIncludeSign || input < 0;

        // Add on the leading zeros
        for (unsigned long int i = before; i < leftDigits; i++) {
            output.insert(insertPos, "0");
        }
        // Return early if no trailing zeros are needed
        if (rightDigits == 0) {return output;}

        // Add a decimal place if it is missing
        if (decimalPos == std::string::npos) {output += ".";}
        // Add on the trailing zeros
        for (unsigned long int i = after; i < rightDigits; i++) {
            output += '0';
        }
        return output;
    }
    /** Convert an arithmetic data type to an std::string with extra leading/trailing zeros if specified
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param leftDigits The minimum amount of digits that will be to the left of the decimal; metric reached by adding leading zeros
     * @param rightDigits The minimum amount of digits that will be to the right of the decimal; metric reached by adding trailing zeros
     * @returns An std::string where each character represents a part of the input; leading and trailing zeros are also included in order to meet certain parameters    */
    template <typename ArithType> std::string tstr_AddZeros(const ArithType &input, const unsigned long int &leadingZeros, const unsigned long int &trailingZeros) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return btils::tstr_AddZeros<ArithType>(input, leadingZeros, trailingZeros, false);
    }

    /** Convert an arithmetic data type to an std::string with a specified minimum length
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param length The minimum length of the output std::string (reached using leading/trailing zeros)
     * @param append Whether to append zeros (trailing zeros) or not (leading zeros) in order to reach the minimum length
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not (sign contributes to the desired length)
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negatie sign; leading or trailing zeros are included to reach a specified minimum string length    */
    template <typename ArithType> std::string tstr_Length(const ArithType &input, const unsigned long int &length, const bool &append, const bool &alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string output = btils::toString<ArithType>(input);

        // Adding leading zeros until the desired length is reached
        if (!append) {
            const bool insertPos = alwaysIncludeSign || input < 0;
            while (output.length() < length) {
                output.insert(insertPos, "0");
            }
            return output;
        }
        
        if (output.find('.') == std::string::npos) {
            // If the output would exceed the desired length by adding on a '.0', then a leading zero is added instead
            if (output.length() >= length - 1) {
                output.insert(alwaysIncludeSign || input < 0, "0");
                return output;
            }
            output += ".0";
        }

        // Add trailing zeros until the desired length is reached
        while (output.length() < length) {
            output += "0";
        }
        return output;
    }
    /** Convert an arithmetic data type to an std::string with a specified minimum length
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param length The minimum length of the output std::string (reached using leading/trailing zeros)
     * @param append Whether to append zeros (trailing zeros) or not (leading zeros) in order to reach the minimum length
     * @returns An std::string where each character represents a part of the input; leading or trailing zeros are included to reach a specified minimum string length    */
    template <typename ArithType> std::string tstr_Length(const ArithType &input, const unsigned long int &length, const bool &append) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return btils::tstr_Length<ArithType>(input, length, append, false);
    }

    /** Convert an arithmetic data type to an std::u16string making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a u16string
     * @returns An std::u16string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions    */
    template <typename ArithType> std::u16string to_u16string(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }
    /** Convert an arithmetic data type to an std::u32string making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a u32string
     * @returns An std::u32string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions    */
    template <typename ArithType> std::u32string to_u32string(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }
    /** Convert an arithmetic data type to an std::wstring making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a wstring
     * @returns An std::wstring where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions    */
    template <typename ArithType> std::wstring to_wstring(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }
    /** Convert an std::string to an std::u16string
     * @param input The std::string to convert from
     * @returns An std::u16string derived from the inputted std::string    */
    template <> std::u16string to_u16string<std::string>(const std::string &input);
    /** Convert an std::string to an std::u32string
     * @param input The std::string to convert from
     * @returns An std::u32string derived from the inputted std::string    */
    template <> std::u32string to_u32string<std::string>(const std::string &input);
    /** Convert an std::string to an std::wstring
     * @param input The std::string to convert from
     * @returns An std::wstring derived from the inputted std::string    */
    template <> std::wstring to_wstring<std::string>(const std::string &input);

//     class UnitInterval {
//         private:
//             double Val;

//             static   bool UseAllowance;
//             static double Allowance;

//         public:
//             /**/                          UnitInterval()                          : Val(0.0) {}
//             /**/                          UnitInterval(const       double &input) : Val(btils::normalize<double>(input, 1.0)) {}
//             /**/                          UnitInterval(const UnitInterval &input) : Val(input.get()) {}
//             template <typename ArithType> UnitInterval(const    ArithType &input) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if (std::is_integral<ArithType>::value) {UnitInterval((double)input * 100.0);}
//                 else {UnitInterval((double)input);}
//             }

//             static double getAllowance()                                  {return Allowance;}
//             static double setAllowance(const double &allowance = 0.00001) {return btils::set<double>(Allowance, allowance);}
//             static double adjAllowance(const double &   amount)           {return btils::adj<double>(Allowance,    amount);}

//             static bool     usesAllowance()                         {return UseAllowance;}
//             static bool setAllowanceUsage(const bool &state = true) {return btils::set<bool>(UseAllowance,         state);}
//             static bool   toggleAllowance()                         {return btils::set<bool>(UseAllowance, !UseAllowance);}

//             /**/                             double get() const {return Val;}
//             template <typename ArithType> ArithType get() const {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 return std::is_integral<ArithType>::value ? (ArithType)std::round(Val * 100.0) : (ArithType)Val;
//             }
//             /**/                             double set(const    double &val) {return btils::set<double>(Val, btils::normalize<double>(val, 1.0));}
//             template <typename ArithType> ArithType set(const ArithType &val) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 return std::is_integral<ArithType>::value ? (ArithType)std::round(set((double)val / 100.0) * 100.0) : (ArithType)set((double)val);
//             }
//             /**/                             double adj(const    double &amount) {return set(Val + amount);}
//             template <typename ArithType> ArithType adj(const ArithType &amount) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 return std::is_integral<ArithType>::value ? (ArithType)std::round(adj((double)amount / 100.0) * 100.0) : (ArithType)adj((double)amount);
//             }

//             template <typename ArithType> operator ArithType() const {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 return std::is_integral<ArithType>::value ? (ArithType)std::round(Val * 100.0) : (ArithType)Val;
//             }
//             UnitInterval  operator!() const {return UnitInterval(1.0 - Val);}
            
//             /**/                          bool operator==(const UnitInterval &rhs) const {return UseAllowance ? Val <= rhs.get() + Allowance && Val >= rhs.get() - Allowance : Val == rhs.get();}
//             /**/                          bool operator!=(const UnitInterval &rhs) const {return !(*this == rhs);}
//             /**/                          bool operator< (const UnitInterval &rhs) const {return     Val <  rhs.get();}
//             /**/                          bool operator> (const UnitInterval &rhs) const {return     rhs <  *this;}
//             /**/                          bool operator<=(const UnitInterval &rhs) const {return !(*this >  rhs);}
//             /**/                          bool operator>=(const UnitInterval &rhs) const {return !(*this <  rhs);}
//             template <typename ArithType> bool operator==(const    ArithType &rhs) const {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 const double comp = std::is_integral<ArithType>::value ? btils::normalize<double>((double)rhs / 100, 1.0) : btils::normalize<double>((double)rhs, 1.0);
//                 return UseAllowance ? Val <= comp + Allowance && Val >= comp - Allowance : Val == comp;
//             }
//             template <typename ArithType> bool operator!=(const    ArithType &rhs) const {return !(*this == rhs);}
//             template <typename ArithType> bool operator< (const    ArithType &rhs) const {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 return Val < (btils::normalize<double>(std::is_integral<ArithType>::value ? (double)rhs / 100 : (double)rhs, 1.0));
//             }
//             template <typename ArithType> bool operator> (const    ArithType &rhs) const {return     rhs <  *this;}
//             template <typename ArithType> bool operator<=(const    ArithType &rhs) const {return !(*this >  rhs);}
//             template <typename ArithType> bool operator>=(const    ArithType &rhs) const {return !(*this <  rhs);}

//             /**/                          UnitInterval& operator+=(const UnitInterval &rhs) {set(          Val + rhs.get());     return *this;}
//             /**/                          UnitInterval& operator-=(const UnitInterval &rhs) {set(          Val - rhs.get());     return *this;}
//             /**/                          UnitInterval& operator*=(const UnitInterval &rhs) {set(          Val * rhs.get());     return *this;}
//             /**/                          UnitInterval& operator/=(const UnitInterval &rhs) {set(          Val / rhs.get());     return *this;}
//             /**/                          UnitInterval& operator%=(const UnitInterval &rhs) {set(std::fmod(Val,  rhs.get()));    return *this;}
//             template <typename ArithType> UnitInterval& operator+=(const    ArithType &rhs) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if   (std::is_integral<ArithType>::value) {set(Val + ((double)rhs / 100.0));}
//                 else                                      {set(Val +  (double)rhs);}
//                 return *this;
//             }
//             template <typename ArithType> UnitInterval& operator-=(const    ArithType &rhs) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if   (std::is_integral<ArithType>::value) {set(Val - ((double)rhs / 100.0));}
//                 else                                      {set(Val -  (double)rhs);}
//                 return *this;
//             }
//             template <typename ArithType> UnitInterval& operator*=(const    ArithType &rhs) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if   (std::is_integral<ArithType>::value) {set(Val * ((double)rhs / 100.0));}
//                 else                                      {set(Val *  (double)rhs);}
//                 return *this;
//             }
//             template <typename ArithType> UnitInterval& operator/=(const    ArithType &rhs) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if   (std::is_integral<ArithType>::value) {set(Val / ((double)rhs / 100.0));}
//                 else                                      {set(Val /  (double)rhs);}
//                 return *this;
//             }
//             template <typename ArithType> UnitInterval& operator%=(const    ArithType &rhs) {
//                 static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
//                 if   (std::is_integral<ArithType>::value) {set(std::fmod(Val, ((double)rhs / 100.0)));}
//                 else                                      {set(std::fmod(Val,  (double)rhs));}
//                 return *this;
//             }

//             /**/                          friend UnitInterval operator+(UnitInterval lhs, const UnitInterval &rhs) {lhs += rhs; return lhs;}
//             /**/                          friend UnitInterval operator-(UnitInterval lhs, const UnitInterval &rhs) {lhs -= rhs; return lhs;}
//             /**/                          friend UnitInterval operator*(UnitInterval lhs, const UnitInterval &rhs) {lhs *= rhs; return lhs;}
//             /**/                          friend UnitInterval operator/(UnitInterval lhs, const UnitInterval &rhs) {lhs /= rhs; return lhs;}
//             /**/                          friend UnitInterval operator%(UnitInterval lhs, const UnitInterval &rhs) {lhs %= rhs; return lhs;}
//             template <typename ArithType> friend UnitInterval operator+(UnitInterval lhs, const    ArithType &rhs) {lhs += rhs; return lhs;}
//             template <typename ArithType> friend UnitInterval operator-(UnitInterval lhs, const    ArithType &rhs) {lhs -= rhs; return lhs;}
//             template <typename ArithType> friend UnitInterval operator*(UnitInterval lhs, const    ArithType &rhs) {lhs *= rhs; return lhs;}
//             template <typename ArithType> friend UnitInterval operator/(UnitInterval lhs, const    ArithType &rhs) {lhs /= rhs; return lhs;}
//             template <typename ArithType> friend UnitInterval operator%(UnitInterval lhs, const    ArithType &rhs) {lhs %= rhs; return lhs;}

//             UnitInterval& operator++()      {adj( 0.01);    return *this;}
//             UnitInterval& operator--()      {adj(-0.01);    return *this;}
//             UnitInterval  operator++(int)   {return btils::adj<UnitInterval>(*this,  0.01);}
//             UnitInterval  operator--(int)   {return btils::adj<UnitInterval>(*this, -0.01);}
//     };
//     double UnitInterval::Allowance    = 0.00001;
//     bool   UnitInterval::UseAllowance = true;
}

#endif /* UTILITIES */
