//
// Created by gortium on 1/15/18.
//

#ifndef PE_TYPES_H
#define PE_TYPES_H

#include "fi/Fixed.hpp"
#include "fi/overflow/Saturate.hpp"
#include "fi/rounding/Classic.hpp"

#define TBIT 8

// bool
typedef Fi::Fixed<1,0,Fi::UNSIGNED,Fi::Saturate,Fi::Classic> typeBool;

// -1.0 to 0.9921875 with 0.0078125 gap
typedef Fi::Fixed<TBIT,7,Fi::SIGNED,Fi::Saturate,Fi::Classic> type7;

// -8.0 to 7.9375 with 0.0625 gap
typedef Fi::Fixed<TBIT,4,Fi::SIGNED,Fi::Saturate,Fi::Classic> type4;

// -4.0 to 0.9921875 with 0.0078125 gap
typedef Fi::Fixed<TBIT,5,Fi::SIGNED,Fi::Saturate,Fi::Classic> type5;

// -4.0 to 0.9921875 with 0.0078125 gap
typedef Fi::Fixed<TBIT,0,Fi::SIGNED,Fi::Saturate,Fi::Classic> type0;

#endif //PE_TYPES_H
