
#include "controllersMK.h"


/************button controllers start****************************/
int buttonState[20]; // 20 buttons can connect
int lastButton[20];
int currentButton[20];
int debounceDelay = 10;
unsigned long previousMillis = 0;
/************button controllers end*******************************/

/************rotary encoder start*********************************/
int currentStateCLK[5]; // 5 rotary encoders can connect
int lastStateCLK[5];
int encoder_counter[5];
// int debounceDelayRot = 100;
// unsigned long previousMillisRot = 0;
// int encoder1_counter = 0;
// int encoder1_address = 50;
/************rotary encoder end**********************************/




/**************************************************************************************************/
/***********PUSH BUTTON AND TOGGLE BUTTON CONTROLLER**********************************************/
/**************************************************************************************************/
uint16_t button_controller(uint8_t buttonGpioPin, uint8_t buttonIndex, uint8_t buttonType) {
  
  
  if (buttonType == 1) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= debounceDelay){
    currentButton[buttonIndex] = digitalRead(buttonGpioPin);
    if (currentButton[buttonIndex] != lastButton[buttonIndex]) {
      lastButton[buttonIndex] = currentButton[buttonIndex];
               previousMillis = currentMillis;
      
      if (lastButton[buttonIndex] == 1) {
        buttonState[buttonIndex] = (buttonState[buttonIndex] == 1) ? 0 : 1;
      }
    }
    }
  } else if (buttonType == 0) {
    buttonState[buttonIndex] = digitalRead(buttonGpioPin);
  }
  return buttonState[buttonIndex];
}


/**************************************************************************************************/
/*************POTENTIOMETERS & jOYSTICKS CONTROL TYPE AND DIRECTIOMN*******************************/
/**************************************************************************************************/
uint16_t potentiometer_controller(uint8_t potGpioPin, uint8_t potValue, uint8_t potDirection){ 
 uint8_t controller = 0;

 uint16_t potReadValue = analogRead(potGpioPin); 
      if(potValue == 180 & potDirection == 0){ controller = map(potReadValue, 0, 4095, 0, 180);}
 else if(potValue == 180 & potDirection == 1){ controller = map(potReadValue, 0, 4095, 180, 0);}
 else if(potValue == 255 & potDirection == 0){ controller = map(potReadValue, 0, 4095, 0, 255);}
 else if(potValue == 255 & potDirection == 1){ controller = map(potReadValue, 0, 4095, 255, 0);}

return controller;

}




/**************************************************************************************************/
/************ROTARY ENCODER***********************************************************************/
/**************************************************************************************************/
uint16_t rotary_encoder_controller(uint16_t encoderCLK, uint16_t encoderDT,uint8_t index,uint16_t rotaryValue, uint8_t rotaryDirection){



  // Read the current state of CLK
    currentStateCLK[index] = digitalRead(encoderCLK);  
      if (currentStateCLK[index] != lastStateCLK[index]  && currentStateCLK[index] == 1){
 

        if(rotaryDirection == 0){//rotate counter clock wise
          if (digitalRead(encoderDT) != currentStateCLK[index]) {        
            encoder_counter[index] --;          
          } else {
            encoder_counter[index] ++;
          }
        }
        else if(rotaryDirection == 1){ //rotate clock wise
          if (digitalRead(encoderDT) != currentStateCLK[index]) {        
            encoder_counter[index] ++;          
          } else {
            encoder_counter[index] --;
          }
        }
      }
    

  if(encoder_counter[index] < 0){
    encoder_counter[index] = 0;
  }
  if(encoder_counter[index] > rotaryValue){
    encoder_counter[index] = rotaryValue;
  }
        // Remember last CLK state
        lastStateCLK[index] = currentStateCLK[index];
        
    
 
  return encoder_counter[index];
}