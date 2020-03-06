#include <Arduino.h>
#include <stdint.h>
#include <hall-switch-ino.h>


#ifdef XMC1100_XMC2GO
#define LED1 14             /**< LED1 Pin Allocation for XMC2Go  */
#else
#define LED1 LED_BUILTIN
#endif

#ifdef XMC1100_XMC2GO 
#define Q_OUTPUT_PIN    9   /**< Hall Switch Output Pin  */
#elif ((XMC1100_Boot_Kit) ||  \
      (XMC4700_Relax_Kit) ||  \
      (ARDUINO_AVR_UNO))
#define Q_OUTPUT_PIN    3   /**< Hall Switch Output Pin  */
#endif

HallSwitch::Status_t stat  = HallSwitch::Status_t::UNINITED;
double               speed = 0.0;
HallSpeedIno         hs(Q_OUTPUT_PIN,1,HallSpeed::HERTZ);

/**
 * @brief       Prints the sensor data in JSON format 
 * @param[in]   stat  status
 * @param[in]   speed Speed value

 */
void JSONPrint(HallSwitch::Status_t stat,
               double speed)
{
  Serial.print("{ \"status\" : ");
  Serial.print(stat);
  Serial.print(", \"speed\" : ");
  Serial.print(speed);
  Serial.println(" }");
}

void setup()
{      
  HallSwitch::Error_t errCode = HallSwitch::Error_t::OK;

  delay(1000);

  Serial.begin(115200);
  Serial.println("Serial initialized");

  pinMode(LED1, OUTPUT);
  Serial.println("LED1 Initialized");

  /* Begin switch hardware interfaces */ 
  errCode = hs.begin();
  if(errCode != HallSwitch::Error_t::OK)
  {
      Serial.print("Hall Speed error: ");
      Serial.println(errCode);
  }
  Serial.println("Hall Speed initialized");

  /* Enable the switch */
  errCode = hs.enable();
  if(errCode != HallSwitch::Error_t::OK)
  {
      Serial.print("Hall Speed error: ");
      Serial.println(errCode);
  }
  Serial.println("Hall Speed enabled");

  JSONPrint(stat, speed);
}

HallSwitch::Status_t curStat  = stat;
double               curSpeed = speed;

void loop()
{
  hs.updateSpeed();
 
  curStat  = hs.getStatus();
  curSpeed = hs.getSpeed();

  if (curStat != stat || curSpeed != speed)
  {
    stat  = curStat;
    if(curSpeed > (speed + 5) )
    {
        digitalWrite(LED1, HIGH);
    }
    speed = curSpeed;
    
    JSONPrint(stat, speed); 
  }
}