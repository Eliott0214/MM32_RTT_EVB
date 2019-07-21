/**************************************************************************//**
 * @file HAL_device.h
 * @brief CMSIS Cortex-M Peripheral Access Layer for MindMotion
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * Example: Add "-DMM32X103" to your build options, to define part
 *          Add "#include "HAL_device.h" to your source files
 *
 *
 * @version 4.2.1
 *
 *
 *****************************************************************************/

#ifndef HAL_device_H
#define HAL_device_H


//#define MM32X103

#if defined(MM32X103)
#include "MM32x103.h"
#include "system_mm32x103.h"

#elif defined(MM32X031)
#include "MM32x031.h"
#include "system_mm32x031.h"

#else

#error "HAL_device.h: PART NUMBER undefined"
#endif
#endif /* HAL_device_H */
