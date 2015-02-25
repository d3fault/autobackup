const int analogInPin = A0;

int sensorValue = 0;
int oldSensorValue = 0;

int halfThresholdForNotifyingOfChange = 2;

void setup()
{
  Serial.begin(9600); 
}

void loop()
{
  //sensorValue = map(analogRead(analogInPin), 0, 1023, 0, 255);       
  sensorValue = analogRead(analogInPin);
  if(sensorValue < (oldSensorValue-halfThresholdForNotifyingOfChange) || sensorValue > (oldSensorValue+halfThresholdForNotifyingOfChange))
  {  
    Serial.println(sensorValue); //TODOoptimization: pretty sure i don't need newlines, but I can't be fucked atm
    oldSensorValue = sensorValue;
  }
  delay(2);
}

