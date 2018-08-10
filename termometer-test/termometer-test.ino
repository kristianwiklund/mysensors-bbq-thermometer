/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue;  // variable to store the value coming from the sensor

// steinhart-hart constants for my probe.
double a=0.0005887972095510851, b=0.00021455581402332063, c=5.958141261835043e-8;



void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int sv=0;
  double res, lt, temp;
  
  for(int i=0;i<10;i++) {
    
  // read the value from the sensor:
  
  sensorValue = analogRead(sensorPin);
  sv=sv+sensorValue;
  delay(10);
  }

  sensorValue=sv/10;
  // turn the ledPin on
  digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  Serial.print("Volts: ");
  Serial.println(sensorValue*5.0/1024.0);
  Serial.print("Ohms: ");

  // vout = (vdd * r2 )/(r1+r2)
  // (r1+r2)*vout = vdd * r2
  // r1*vout = vdd*r2-vout*r2
  // r1 = (vdd-vout)*r2/vout
  // now add the equation for the resistance and eliminate vdd

  
  
  res = ((1.0-sensorValue/1024.0)*47000/(sensorValue/1024.0)); // this is our probe resistance.
  Serial.println(res);
  Serial.println("---");

  // calculate the actual temperature

  lt = log(res);
  temp = 1 / (a+b*lt+c*lt*lt*lt); // in kelvin
  Serial.print("Kelvin: ");
  Serial.println(temp);
  Serial.print("Celsius: ");
  Serial.println(temp-273.15);
  
  delay(sensorValue);
  // turn the ledPin off:
  digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  delay(sensorValue);
}
