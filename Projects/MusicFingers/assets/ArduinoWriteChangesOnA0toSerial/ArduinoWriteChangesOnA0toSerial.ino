#define MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) analogInPin_##pin
#define MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) pin##_sensorValue
#define MUSIC_FINGERS_OLD_SENSOR_VALUE(pin) pin##_oldSensorValue

#define MUSIC_FINGER_GLOBALS(pin) \
const int MUSIC_FINGRERS_ANALOG_PIN_VAR(pin) = pin; \
int MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) = 0; \
int MUSIC_FINGERS_OLD_SENSOR_VALUE(pin) = 0;


MUSIC_FINGER_GLOBALS(A0)
MUSIC_FINGER_GLOBALS(A1)


#define MUSIC_FINGERS_MAIN_LOOP(pinQuoted, pin) \
MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) = analogRead(MUSIC_FINGRERS_ANALOG_PIN_VAR(pin)); \
if(MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) < (MUSIC_FINGERS_OLD_SENSOR_VALUE(pin)-halfThresholdForNotifyingOfChange) || MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin) > (MUSIC_FINGERS_OLD_SENSOR_VALUE(pin)+halfThresholdForNotifyingOfChange)) \
{ \
  Serial.print(pinQuoted); \
  Serial.print(":"); \
  Serial.println(MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin)); /*TODOoptimization: pretty sure i don't need newlines, but I can't be fucked atm*/ \
  MUSIC_FINGERS_OLD_SENSOR_VALUE(pin) = MUSIC_FINGERS_NEW_SENSOR_VALUE_VAR(pin); \
}


int halfThresholdForNotifyingOfChange = 2;

void setup()
{
  Serial.begin(9600);
}
void loop()
{
  MUSIC_FINGERS_MAIN_LOOP("0", A0)
  MUSIC_FINGERS_MAIN_LOOP("1", A1)  
  
  delay(2);
}

