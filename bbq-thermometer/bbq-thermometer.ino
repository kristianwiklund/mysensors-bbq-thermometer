/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Original sketch (mysensors demo for LightSensor was Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 * Sketch for homemade bbq thermometer using an IKEA FANTAST probe 
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#include <MySensors.h>

#define CHILD_ID_PROBE1 0
#define PROBE1_ANALOG_PIN 0

uint32_t SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

MyMessage msg(CHILD_ID_PROBE1, V_TEMP);

// steinhart-hart constants for my probe.
double a=0.0005887972095510851, b=0.00021455581402332063, c=5.958141261835043e-8;

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("BBQ Thermometer", "1.0");

	// Register all sensors to gateway (they will be created as child devices)
	present(CHILD_ID_PROBE1, S_TEMP);
}

double readtemperature(int sensorPin) {
  int sv=0;
  double res, lt, temp;
  int sensorValue;
  
  for(int i=0;i<10;i++) {
    
  // read the value from the sensor:
  
  sensorValue = analogRead(sensorPin);
  sv=sv+sensorValue;
  delay(10);
  }

  sensorValue=sv/10;

  res = ((1.0-sensorValue/1024.0)*47000/(sensorValue/1024.0)); // this is our probe resistance.


  // calculate the actual temperature

  lt = log(res);
  temp = 1 / (a+b*lt+c*lt*lt*lt); // in kelvin
  temp = temp-273.15; // in celsius

  return temp;
}

void loop()
{
	double temperature = readtemperature(PROBE1_ANALOG_PIN);
  
	Serial.println(temperature);
  send(msg.set((float)temperature,1));
	sleep(SLEEP_TIME);
}



