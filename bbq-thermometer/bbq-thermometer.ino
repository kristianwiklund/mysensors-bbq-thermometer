

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
#define CHILD_ID_SETPOINT 1
#define PIEZOPIN A3 
#define CHILD_ID_HVAC CHILD_ID_SETPOINT

uint32_t SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)
float mysetpoint = 55.0;

MyMessage msg(CHILD_ID_PROBE1, V_TEMP);
MyMessage msgHVACTemp(CHILD_ID_SETPOINT, V_TEMP);
MyMessage msgHVACSetPointC(CHILD_ID_SETPOINT, V_HVAC_SETPOINT_HEAT);
// dummy messages
MyMessage msgHVACSpeed(CHILD_ID_HVAC, V_HVAC_SPEED);
MyMessage msgHVACFlowState(CHILD_ID_HVAC, V_HVAC_FLOW_STATE);
// steinhart-hart constants for my probe.
double a=0.0005887972095510851, b=0.00021455581402332063, c=5.958141261835043e-8;

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("BBQ Thermometer", "1.0");

	// Register all sensors to gateway (they will be created as child devices)
	present(CHILD_ID_PROBE1, S_TEMP, "Temperature",true);
  present(CHILD_ID_SETPOINT, S_HVAC, "Setpoint",true);

}

float readtemperature(int sensorPin) {
  int sv=0;
  double res, lt, temp;
  int sensorValue;
  
  for(int i=0;i<10;i++) {
    
  // read the value from the sensor:
  
  sensorValue = analogRead(sensorPin);
  sv=sv+sensorValue;
  wait(10);
  }

  sensorValue=sv/10;
  Serial.println(sensorValue);
  
  res = ((1.0-sensorValue/1024.0)*47000/(sensorValue/1024.0)); // this is our probe resistance.


  // calculate the actual temperature

  lt = log(res);
  temp = 1 / (a+b*lt+c*lt*lt*lt); // in kelvin
  Serial.println(temp);
  temp = temp-273.15; // in celsius
  Serial.println(temp);

  return temp;
}

bool initialValueSent = false;
String HA_MODE_STATE = "HeatOn";
String HA_FAN_STATE = "Max";

int lastsend=0;

bool alarm=false, beeping = false;

void loop()
{
	double temperature;

 if (!initialValueSent) {

    temperature = readtemperature(PROBE1_ANALOG_PIN);
    send(msgHVACSetPointC.set(mysetpoint,1));
    
     char fan_state[HA_FAN_STATE.length() + 1];
  HA_FAN_STATE.toCharArray(fan_state, HA_FAN_STATE.length() + 1);
  send(msgHVACSpeed.set(fan_state));
    char mode_state[HA_MODE_STATE.length() + 1];
    HA_MODE_STATE.toCharArray(mode_state, HA_MODE_STATE.length() + 1);
    send(msgHVACFlowState.set(mode_state));

    initialValueSent = true;
    Serial.println("hello sent");
 }

lastsend++;
if (lastsend > 10) {
    temperature = readtemperature(PROBE1_ANALOG_PIN);

	Serial.println(temperature);
  send(msg.set((float)temperature,1));
  send(msgHVACTemp.set((float)temperature,1));
  lastsend = 0;
  if (temperature >= mysetpoint) 
    alarm = true;
  else
    alarm = false;
}

  if(alarm) {

    if (beeping) {
      tone(PIEZOPIN, 800, SLEEP_TIME/10);
      Serial.println("pip");
    } else {
      Serial.println("opip");
    }
  beeping = !beeping;
  }
  wait(SLEEP_TIME/10);

 
}

void receive(const MyMessage &message) {

  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
     return;
  }

  Serial.print("Incoming message for: ");
  Serial.print(message.sensor);

  String recvData = message.data;
  recvData.trim();

  Serial.print(", New status: ");
  Serial.println(recvData);
  switch (message.type) {

    case V_HVAC_SETPOINT_HEAT:
    case V_HVAC_SETPOINT_COOL:
    
      Serial.println("V_HVAC_SETPOINT_...");
      mysetpoint = message.getFloat();
      Serial.println(mysetpoint);
      tone(PIEZOPIN, 8000, 600);

    break;
  }
}




