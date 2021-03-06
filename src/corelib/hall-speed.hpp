/** 
 * @file        hall-speed.hpp
 * @brief       Hall Speed API
 * @date        March 2020
 * @copyright   Copyright (c) 2019-2020 Infineon Technologies AG
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HALL_SPEED_HPP_
#define HALL_SPEED_HPP_

#include "../config/hall-conf.hpp"
#if (HALL_SPEED_ENABLED == 1)

#include "hall-switch.hpp"

/**
 * @addtogroup hspapi
 * @{
 */ 

class HallSpeed: public HallSwitch
{           
    public:
        
        /**
         * @name Speed units
         * @{
         */
         enum SpeedUnit_t
         {
            HERTZ   = 0,    /**< cps - Hertz */
            RADS    = 1,    /**< rads/s */  
            RPM     = 2     /**< RPM */
         };
        /** @} */

        /**
         * @brief       Callback function type for interrupt mode 
         * @param[in]   speed The new speed change. 
         * @return      void
         */
        typedef void (*CBackSpd_t)(double);

        /**
         * @brief Timer Interface
         */
        class Timer;

                    HallSpeed   ();
                    HallSpeed   (GPIO        *output,
                                 Timer       *timer,
                                 uint8_t     polesNum = 1,
                                 SpeedUnit_t units    = HERTZ,
                                 CBackSpd_t  cBack    = NULL,
                                 GPIO        *power   = NULL);
                   ~HallSpeed   ();
        Error_t     init        ();
        Error_t     deinit      ();
        Error_t     enable      ();
        Error_t     disable     ();                         
        Error_t     updateSpeed ();
        double      getSpeed    ();

    protected:
        Timer       *timer;                  /**< Timer interface pointer */
        uint8_t     polesPair;               /**< Rotor poles pair number */
        SpeedUnit_t sUnits;                  /**< Speed units */
        double      speed;                   /**< Rotating speed */

        static double const speedCoeff[3];

        void        callback        ();
        void        calculateSpeed  ();
};

/** @} */

#endif /** HALL_SPEED_ENABLED */
#endif /** HALL_SPEED_HPP_ **/