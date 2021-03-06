/** 
 * @file        hall-switch.cpp
 * @brief       Hall Switch API
 * @date        July 2019
 * @copyright   Copyright (c) 2019 Infineon Technologies AG
 * 
 * SPDX-License-Identifier: MIT
 */

#include "hall-switch.hpp"
#include "../pal/hall-pal-gpio.hpp"

/**
 * @brief   Hall Switch Default Constructor
 *          - Hardware interfaces pointers set to NULL
 *          - Default modes polling (for measure mode) and main (for switch power mode)      
 * @pre    None
 */
HallSwitch::HallSwitch()
{
    status           = UNINITED;
    output           = NULL;
    bfieldVal        = B_FIELD_UNDEF;

    cBack            = NULL;
    measMode         = POLLING;
    
    power            = NULL;
    powerMode        = MAIN;
};

/**
 * @brief       Hall Switch Constructor
 * 
 *   Mandatory arguments: 
 *      - Sensor output GPIO pointer
 * 
 *   Optional  arguments: 
 *      - Interrupt callback. By default NULL. Required to enable interrupt mode.
 *      - Sensor power GPIO pointer (only for switch powered mode platform). By default it is NULL.
 * 
 * @param[in]   *output Sensor output GPIO interface pointer
 * @param[in]   cBack   Callback for interrupt mode. When passed, it enables interrupt mode
 * @param[in]   *power  Sensor switch power controller GPIO interface pointer. Default NULL will set power mode to MAIN
 * @pre    None
 */
HallSwitch::HallSwitch(GPIO         *output,
                       CBack_t      cBack,
                       GPIO         *power)
{
    status              = UNINITED;
    this->output        = output;
    bfieldVal           = B_FIELD_UNDEF;

    this->cBack = cBack;
    if(cBack == NULL)
        this->measMode  = POLLING;
    else
        this->measMode  = INTERRUPT;

    this->power         = power;
    if(this->power == NULL)
        this->powerMode = MAIN;
    else
        this->powerMode = SWITCH;
}

/**
 * @brief   Hall Switch Destructor
 *          Disables the sensor:
 *              - If "interrupt measuring mode" is enabled, the interrupt is disabled
 *              - If "switch power mode" is configured, the sensor power is disabled          
 * @pre    None
 */
HallSwitch::~HallSwitch()
{
    if(status == POWER_ON)
        disable();
    if(status == POWER_OFF)
        deinit();
}

/**
 * @brief   Initializes the hardware interfaces
 * @pre     None
 * @return  HallSwitch error code
 * @retval  OK if success
 * @retval  INTF_ERROR if hardware interface error
 */
HallSwitch::Error_t  HallSwitch::init()
{
    Error_t err = OK;

    if(powerMode == SWITCH)
    {   
        if(power == NULL)
            return INTF_ERROR;
        
        err = power->init();
        if(err != OK) return err;        
    }

    if(output == NULL)
      return INTF_ERROR;

    err = output->init();
    if(err != OK) return err;
    
    status = INITED;

    return err;
}

/**
 * @brief   Deinitializes the hardware interfaces
 * @pre     Instance has called init()
 * @pre     Instance has called disable() if enable() was called
 * @return  HallSwitch error code
 * @retval  OK if success
 * @retval  INTF_ERROR if hardware interface error
 */
HallSwitch::Error_t  HallSwitch::deinit()
{
    Error_t err = OK;

    if(powerMode == SWITCH)
    {
        if(power == NULL)
            return INTF_ERROR;

        err = power->deinit();
        if(err != OK) return err;
    }

    if(output == NULL)
      return INTF_ERROR;

    err = output->deinit();
    if(err != OK) return err;

    status    = UNINITED;
    bfieldVal = B_FIELD_UNDEF;

    return err;
}

/**
 * @brief   Enables the sensor
 *          - If "switch power mode " is configured, the sensor is powered up
 *          - If "interrupt measuring mode" is configured, the interrupt is enabled
 * 
 * @pre     Instance has called init()
 * @return  HallSwitch error code 
 * @retval  OK if success
 * @retval  INTF_ERROR if hardware interface error     
 */
HallSwitch::Error_t  HallSwitch::enable()
{
    Error_t err = OK;

    if(powerMode == SWITCH)
    {
        err = power->enable();
        if(err != OK) return err;
    }

    if(measMode == INTERRUPT)
    {
       err = output->enableInt(this);
       if(err != OK) return err;
    }

    status = POWER_ON;

    return err;
}

/**
 * @brief   Disables the sensor
 *              - If the "interrupt measuring mode" is configured, the interrupt is disabled 
 *              - If the "switch power mode" is configured, the sensor is powered off
 
 * @pre     Instance has called enable()
 * @return  HallSwitch error code 
 * @retval  OK if success
 * @retval  INTF_ERROR if hardware interface error     
 */
HallSwitch::Error_t  HallSwitch::disable()
{
    Error_t err = OK;

    if(measMode == INTERRUPT)
    {
        err = output->disableInt();
        if(err != OK) return err;
    }

    if(powerMode == SWITCH)
    {
        err = power->disable();
        if(err != OK) return err;
    }
    status = POWER_OFF;

    return err;
}

/**
 * @brief   Updates instance magnetic field value 
 * 
 *  - Magnetic field present if the GPIO voltage level is low
 *  - Mangnetic field not present if the GPIO voltage level is high
 * 
 *  The "bfieldVal" object member is updated with the read GPIO value.
 * 
 * @pre     Instance has called enable()
 * @return  HallSwitch error code 
 * @retval  OK if success
 * @retval  INTF_ERROR if hardware interface error     
 */
HallSwitch::Error_t HallSwitch::updateBField()
{
	GPIO::VLevel_t gpiolevel = output->read();   

	if(gpiolevel == GPIO::VLevel_t::GPIO_LOW)
    {
        bfieldVal = B_FIELD_ON;
	}
    else if(gpiolevel == GPIO::VLevel_t::GPIO_HIGH)
    {
        bfieldVal = B_FIELD_OFF;
	}

    return OK;
}

/**
 * @brief   Gets the sensor status
 * @return  HallSwitch status
 */
HallSwitch::Status_t HallSwitch::getStatus()
{
    return status;
}

/**
 * @brief   Gets magnetic field 
 * 
 *  This functions returns the last read value updated in the object member bfieldVal variable,
 *  either from a explicit updateBField() call or due to an interrupt event.
 * 
 * @pre     If polling mode is used, updateBField() has to be called preivously in order to get the actual value.
 *  
 * @return  HallSwitch Result_t magnetic field value
 * @retval  B_FIELD_ON if magnetic field present
 * @retval  B_FIELD_OFF if magnetic field NOT present
 */
HallSwitch::Result_t HallSwitch::getBField()
{
    return bfieldVal;
}

/**
 * @brief   Interrupt mode callback function
 *  
 *  The instance updates on its own bfieldVal member when the interrupt occurs.
 * 
 *  Additionally, the user callback function is executed. 
 * 
 * @return  void
 */
void HallSwitch::callback()
{
    GPIO::IntEvent_t event = output->intEvent();

    if(event == GPIO::IntEvent_t::INT_FALLING_EDGE)
    {
        bfieldVal = B_FIELD_ON;
    }
    else if(event == GPIO::IntEvent_t::INT_RISING_EDGE)
    {
       bfieldVal = B_FIELD_OFF;
    }

    cBack(bfieldVal);
}




