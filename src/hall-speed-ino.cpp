/** 
 * @file        hall-speed-ino.cpp
 * @brief       Hall Speed Arduino Class
 *                  
 *  Arduino wrapper for hall speed hardware abstraction layer
 *      - Hardware Allocation using direct arduino pins
 *      - Predefined Arduino based hardware platforms: Shield2Go + (XMC Arduino / Arduino UNO)
 * 
 * @date        March 2020
 * @copyright   Copyright (c) 2019-2020 Infineon Technologies AG
 */

#include "hall-speed-ino.h"
/**
 * @brief           Hall speed ino instance constructor with Arduino pins
 *                  Mandatory arguments: 
 *                      - sensor output pin
 *                  Optional  arguments: 
 *                      - interrupt callback. By default NULL. Required to enable interrupt mode.
 *                      - sensor power pin (only for switch powered mode platform). By default it is UNUSED.
 *                 
 * @param[in]       outputPin   Hall switch output pin
 * @param[in]       cBack       Callback for interrupt mode. When passed, it enables interrupt mode.  
 * @param[in]       powerPin    Hall switch power pin. When passed, enabled the sensor switch controlled mode.
 * @return          void         
 */
HallSpeedIno::HallSpeedIno(uint8_t     outputPin,
                           uint8_t     polesNum,
                           SpeedUnit_t units,
                           CBackSpd_t  cBack,
                           uint8_t     powerPin)
:HallSpeed(new GPIOIno(outputPin, INPUT_PULLUP, GPIO::VLogic_t::POSITIVE), 
           new TimerIno(),
           polesNum,
           units,
           cBack,
           (powerPin == UNUSED_PIN) ? NULL : new GPIOIno(outputPin, OUTPUT, GPIO::VLogic_t::POSITIVE)){ }

/**
 * @brief           Hall speed ino instance constructor with predefined Arduino hardware platform
 *                  Mandatory arguments: 
 *                      - Hardware platform 
 *                  Optional  arguments: 
 *                      - interrupt callback. By default NULL. Required to enable interrupt mode.
 *                  
 * @param[in]       hwPlatf     Predefined Arduino hardware platform.
 * @param[in]       cBack       Callback for interrupt mode. When passed, it enables interrupt mode.  
 * @return          void         
 */
HallSpeedIno::HallSpeedIno(Platform_t  hwPlatf,
                           uint8_t     polesNum,
                           SpeedUnit_t units,
                           CBackSpd_t  cBack)
:HallSpeed(new GPIOIno(hwPlatf.output, INPUT_PULLUP, GPIO::VLogic_t::POSITIVE), 
           new TimerIno(),
           polesNum,
           units,
           cBack,
           (hwPlatf.power == UNUSED_PIN) ? NULL : new GPIOIno(hwPlatf.power, OUTPUT, GPIO::VLogic_t::POSITIVE)){ }