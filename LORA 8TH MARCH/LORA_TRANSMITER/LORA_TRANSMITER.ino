 #include <ArduinoJson.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>




/******LORA START*****************************************/
//lora esp32
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 4;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xFF;     // address of this device
byte destination = 0xBB;      // destination to send to
unsigned long previousMillisTX = 0;
/******LORA END*****************************************/


/*********controllers gpio pins start***********/
#define nextPageButton 15
#define MainMenuButton 16
#define SubMenuButton 17

#define Buzzer 3
#define joystick1 36
#define joystick2 39
#define joystick3 34
#define joystick4 35
#define potentiometer1 32
#define potentiometer2 33
#define potentiometer3 26
#define potentiometer4 25

#define joyButton1 14
#define joyButton2 27
#define switch1 12
#define switch2 13
///*********controllers gpio pins end***********/

/************button controllers start****************************/
int buttonState[10];
int lastButton[10];
int currentButton[10];
int debounceDelay = 10;
unsigned long previousMillis = 0;

int button1State = 0;
int button2State = 0;
int switch1State = 0;
int switch2State = 0;
int pageDown = 0;
int pageUp = 0;
int menu = 0;
int subMenu = 0;

/************button controllers end*******************************/

int lastButton1 = 0;
int lastButton2 = 0;


/********EEPROM memory start***********************/
int pot1_address = 1;
int pot2_address = 2;
int pot3_address = 3;
int pot4_address = 4;
int joy1_address = 5;
int joy2_address = 6;
int joy3_address = 7;
int joy4_address = 8;

int pot1Dir_address = 9;
int pot2Dir_address = 10;
int pot3Dir_address = 11;
int pot4Dir_address = 12;
int joy1Dir_address = 12;
int joy2Dir_address = 14;
int joy3Dir_address = 15;
int joy4Dir_address = 16;

int btn1Dir_address = 17;
int btn2Dir_address = 18;
int btn3Dir_address = 19;
int btn4Dir_address = 20;
int pageCount_address = 21;


byte pot1_value, pot2_value, pot3_value, pot4_value;
byte joy1_value, joy2_value, joy3_value, joy4_value;
byte pot1Dir_value, pot2Dir_value, pot3Dir_value, pot4Dir_value;
byte joy1Dir_value, joy2Dir_value, joy3Dir_value, joy4Dir_value;
byte btn1Dir_value,btn2Dir_value,btn3Dir_value,btn4Dir_value;
/***************EEPROM memory end***********/


/********LORA RECEIVE*********/
//const char* fwVersionSlave1;
int connectionStatus = 0;
int rsi, snr;
float tmp;
int humi;
int btn1Status,btn2Status,sw1Status,sw2Status;
int pot1Status,pot2Status,pot3Status,pot4Status;;


void buttonsAndCotrollers(void *pvParameters);
void memorySaving();


int pot1cursor, pot2cursor,pot3cursor, pot4cursor, joy1cursor, joy2cursor,joy3cursor, joy4cursor;
int pot1_level = 0;
int pot2_level = 0; 
int pot3_level = 0;
int pot4_level = 0;

int joy1_level = 0;
int joy2_level = 0;
int joy3_level = 0;
int joy4_level = 0;


String firmwareVersionMaster = "v1.0.1";

String connection; 
String btn1Dir_Type;
String btn2Dir_Type;
String btn3Dir_Type;
String btn4Dir_Type;
String pot1Dir_Type;
String pot2Dir_Type;
String pot3Dir_Type;
String pot4Dir_Type;
String joy1Dir_Type;
String joy2Dir_Type;
String joy3Dir_Type;
String joy4Dir_Type;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

lora_init();

  xTaskCreatePinnedToCore(
    buttonsAndCotrollers
    ,  "buttonsAndCotrollers"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  1);

}



void loop() {
   //sending data to slave node lora
  lora_send_data();
  
  onReceive(LoRa.parsePacket());
  



}




void lora_send_data(){

  

  String SendJSONData;

  
    StaticJsonDocument<512> LORA_TX;
//DynamicJsonDocument LORA_TX(200);


   JsonArray button = LORA_TX.createNestedArray("btn");
    button.add(button1State);
    button.add(button2State);
    button.add(switch1State);
    button.add(switch2State);       
    JsonArray potentiometers = LORA_TX.createNestedArray("pot");
    potentiometers.add(pot1_level);
    potentiometers.add(pot2_level);
    potentiometers.add(pot3_level);
    potentiometers.add(pot4_level);
    JsonArray joysticks = LORA_TX.createNestedArray("joy");
    joysticks.add(joy1_level);
    joysticks.add(joy2_level);
    joysticks.add(joy3_level);
    joysticks.add(joy4_level);




//JsonArray sendData = LORA_TX.createNestedArray("TXdata");
//sendData.add(button1State);//0
//sendData.add(button2State);//1
//sendData.add(switch1State);//2
//sendData.add(switch2State);//3
//sendData.add(pot1_level);//4
//sendData.add(pot2_level);//5
//sendData.add(pot3_level);//6
//sendData.add(pot4_level);//7
//sendData.add(joy1_level);//8
//sendData.add(joy2_level);//9
//sendData.add(joy3_level);//10
//sendData.add(joy4_level);//11


serializeJson(LORA_TX, SendJSONData);
//serializeJsonPretty(LORA_TX, SendJSONData);







  unsigned long currentMillisTX = millis();
  if (currentMillisTX - previousMillisTX >= 10) {
    previousMillisTX = currentMillisTX;
     
      Serial.println("Sending:" + SendJSONData);
      LoRa.beginPacket();                   // start packet
      LoRa.write(destination);              // add destination address
      LoRa.write(localAddress);             // add sender address
      LoRa.write(msgCount);                 // add message ID
      LoRa.write(SendJSONData.length());        // add payload length
      LoRa.print(SendJSONData);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      msgCount++;                           // increment message ID
      }

      
 }  





void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
    connectionStatus = 1;
  }
//  Serial2.println(incoming);//UART TRANSMMIT

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xBB) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }


StaticJsonDocument<512> LORA_RX;
  DeserializationError error = deserializeJson(LORA_RX, incoming);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  
//  const char* sensor = LORA_RX["sensor"];
//  long time = LORA_RX["time"];

//  fwVersionSlave1 = LORA_RX["firmware"];
  rsi = LORA_RX["rssi"];
  snr = LORA_RX["snr"];
  tmp = LORA_RX["temp"];
  humi = LORA_RX["humi"];
  btn1Status = LORA_RX["btnState"][0];
  btn2Status = LORA_RX["btnState"][1];
  sw1Status = LORA_RX["btnState"][2];
  sw2Status = LORA_RX["btnState"][3];
  pot1Status = LORA_RX["potState"][0];
  pot2Status = LORA_RX["potState"][1];
  pot3Status = LORA_RX["potState"][2];
  pot4Status = LORA_RX["potState"][3];
  
  // if message is for this device, or broadcast, print details:
  Serial.print("Received from: 0x" + String(sender, HEX));
  Serial.print(", Sent to: 0x" + String(recipient, HEX));
  Serial.print(", Message ID: " + String(incomingMsgId));
  Serial.print(", Message length: " + String(incomingLength));
  Serial.print(", RSSI: " + String(LoRa.packetRssi()));
  Serial.println(", Snr: " + String(LoRa.packetSnr()));
  Serial.println("Incoming Message: " + incoming);
  Serial.println();
  
}





 



void lora_init(){
      Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}


void buttonsAndCotrollers(void *pvParameters){

  (void) pvParameters;
pinMode(potentiometer1, INPUT);
pinMode(potentiometer2, INPUT);
pinMode(potentiometer3, INPUT);
pinMode(potentiometer4, INPUT);
pinMode(joystick1, INPUT);
pinMode(joystick2, INPUT);
pinMode(joystick3, INPUT);
pinMode(joystick4, INPUT);

pinMode(joyButton1, INPUT_PULLUP);
pinMode(joyButton2, INPUT_PULLUP);
pinMode(switch1, INPUT_PULLUP);
pinMode(switch2, INPUT_PULLUP);

  for (;;)
  { 
    pot1_level = adc_controller_dynamic_mapping(180, 0, potentiometer1);
    pot2_level = adc_controller_dynamic_mapping(180, 1, potentiometer2);
    pot3_level = adc_controller_dynamic_mapping(180, 0, potentiometer3);
    pot4_level = adc_controller_dynamic_mapping(180, 1, potentiometer4);
    joy1_level = adc_controller_dynamic_mapping(255, 0, joystick1);
    joy2_level = adc_controller_dynamic_mapping(255, 1, joystick2);
    joy3_level = adc_controller_dynamic_mapping(255, 0, joystick3);
    joy4_level = adc_controller_dynamic_mapping(255, 1, joystick4);
  
    button1State = button_controller_status(joyButton1, 1, 1);
    button2State = button_controller_status(joyButton2, 2, 1);
    switch1State = button_controller_status(switch1, 3, 0);
    switch2State = button_controller_status(switch2, 4, 0);
  
vTaskDelay(1);
  }
}


/*************POTENTIOMETERS & jOYSTICKS CONTROL TYPE AND DIRECTIOMN*******************************/
uint16_t adc_controller_dynamic_mapping(uint8_t potValue, uint8_t potDirection, uint8_t potGpioPin){ 
  uint8_t controller = 0;
  uint16_t potReadValue = analogRead(potGpioPin); 
       if(potValue == 180 & potDirection == 0){ controller = map(potReadValue, 0, 4095, 0, 180);}
  else if(potValue == 180 & potDirection == 1){ controller = map(potReadValue, 0, 4095, 180, 0);}
  else if(potValue == 255 & potDirection == 0){ controller = map(potReadValue, 0, 4095, 0, 255);}
  else if(potValue == 255 & potDirection == 1){ controller = map(potReadValue, 0, 4095, 255, 0);}
return controller;
}

/************POTENTIOMETERS & JOYSTICKS CURSOR POSITION CONTROLLER*********************************/
uint16_t adc_controller_cursor_position(uint8_t value, uint8_t directionValue, uint8_t gpioLevel){
    uint8_t cursorPosition; 
       if(value == 180 & directionValue == 0){ cursorPosition = map(gpioLevel, 180, 0, 0, 120);}
  else if(value == 180 & directionValue == 1){ cursorPosition = map(gpioLevel, 180, 0, 120, 0);}
  else if(value == 255 & directionValue == 0){ cursorPosition = map(gpioLevel, 255, 0, 0, 120);}
  else if(value == 255 & directionValue == 1){ cursorPosition = map(gpioLevel, 255, 0, 120, 0);}
return cursorPosition;
}

/***********PUSH BUTTON AND TOGGLE BUTTON CONTROLLER**********************************************/
uint16_t button_controller_status(uint8_t buttonGpioPin, uint8_t buttonIndex, uint8_t buttonType) {
  if (buttonType == 1) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= debounceDelay){
    currentButton[buttonIndex] = digitalRead(buttonGpioPin);
    if (currentButton[buttonIndex] != lastButton[buttonIndex]) {
      lastButton[buttonIndex] = currentButton[buttonIndex];
               previousMillis = currentMillis;
      
      if (lastButton[buttonIndex] == 0) {
        buttonState[buttonIndex] = (buttonState[buttonIndex] == 0) ? 1 : 0;
      }
    }
    }
  } else if (buttonType == 0) {
    buttonState[buttonIndex] = digitalRead(buttonGpioPin);
  }
  return buttonState[buttonIndex];
}
