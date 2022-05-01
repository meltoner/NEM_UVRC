/*
  Steer.h - Library for wraping the functions controling the Steer
  Created Konstantinos Papageorgiou  
*/

#include "Arduino.h"
#include "Steer.h"
#include <Servo.h>
#include "Remote.h"
#include "Context.h"

Steer::Steer(int pin){  
  _pin = pin;
}

void Steer::setup(Context &_context, Remote &_remote){
  context = &_context;
  remote = &_remote;
  steer.attach(_pin);
  on = true;
  setSteer(center);
  Serial.println("Steer ready.");
}

int Steer::getHeadingDifference(){
  if(context->derivatives[2] > 30)
   return 30;
  if(context->derivatives[2] < -30)
   return -30;
  return (int)(context->derivatives[2] * 0.95);
}

void Steer::setSteer(int value){
  if(context->actuators[0] != value){
    applied = context->now;
    steer.write(context->actuators[0] = value);  
  }
}

void Steer::apply(){

  // steering angle derivation
  int value = center;  
  if(remote->isSwitchA())
    value = value - getHeadingDifference();
  else
    value = value - map(context->ext_sensors[0], 0, 255, -50, 50);

 // save power when reached center
  if(context->actuators[0] == value && abs(value - center)<4 ){
    if(on && (context->now - applied) > 1000  ){
      Serial.print("stopping");
      steer.detach();
      on = false;      
    }
  }else{
    if(!on){
      steer.attach(_pin);
      on = true;
    }
  }

  if(on)
    setSteer( value );

}

boolean Steer::hasNewDegree(){
  if(remote->isSwitchB())
    context->targets[0] = context->derivatives[1];
}