#pragma once

#include_next <mcuconf.h>

// This line stops the "RP2040_MCUCONF not defined" error
#define RP2040_MCUCONF

// These lines stop the "I2C0/I2C1 not defined" errors
#undef RP_I2C_USE_I2C0
#define RP_I2C_USE_I2C0 FALSE

#undef RP_I2C_USE_I2C1
#define RP_I2C_USE_I2C1 TRUE