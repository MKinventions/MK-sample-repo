#include <ArduinoJson.h>
#include <EEPROM.h>
#include "EEPROM.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "controllersMK.h"//custom library

/*********OLED DISPLAY START*********************************************/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/**********OLED DISPLAY END****************************************************/





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



int pagesCount = 0;
int MainMenuSelect = 0;
int SubMenuSelect = 0;
int totalPages = 7;
int totalMenus = 10;
int totalSubMenus = 6;

/*********controllers gpio pins start***********/
#define nextPageButton 15
#define MainMenuButton 16
#define SubMenuButton 17
#define sendDataButton 1

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


int button1State = 0;
int button2State = 0;
int switch1State = 0;
int switch2State = 0;
int statusButton = 0;
int pageDown = 0;
int pageUp = 0;
int menu = 0;
int subMenu = 0;




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
int joy1Dir_address = 13;
int joy2Dir_address = 14;
int joy3Dir_address = 15;
int joy4Dir_address = 16;
int btn1Dir_address = 17;
int btn2Dir_address = 18;
int btn3Dir_address = 19;
int btn4Dir_address = 20; 
int pageCount_address = 21;
int dataSpeed_address = 22;
int communication_address = 23;
int radio_address = 24; 
int controlMode_address = 25;

byte pot1_value, pot2_value, pot3_value, pot4_value; 
byte joy1_value, joy2_value, joy3_value, joy4_value;
byte pot1Dir_value, pot2Dir_value, pot3Dir_value, pot4Dir_value; 
byte joy1Dir_value, joy2Dir_value, joy3Dir_value, joy4Dir_value;
byte btn1Dir_value,btn2Dir_value;
byte dataSpeed_value = 3;
byte communication_value = 3;
byte radio_value = 0;
byte controlMode_value;
/***************EEPROM memory end***********/


int joy1CalibrateValue;
int joy1CalibrationAddress = 0x80;
int joy2CalibrateValue;
int joy2CalibrationAddress = 0x81;
int joy3CalibrateValue;
int joy3CalibrationAddress = 0x82;
int joy4CalibrateValue;
int joy4CalibrationAddress = 0x83;
int calibrationButton = 0;
int calibration_address = 0x84;
int joyOffsetValue = 2;
int joyOffsetAddress = 0x85;


/********LORA RECEIVE*********/
//const char* fwVersionSlave1;
int connectionStatus = 0;
int rsi, snr;
float tmp;
int humi;
int btn1Status,btn2Status,sw1Status,sw2Status;
int pot1Status,pot2Status, pot3Status, pot4Status;
double latitude;
double longitude;
double voltage;
double distance;
float accelarometerX;
float accelarometerY;



int pot1cursor, pot2cursor, pot3cursor, pot4cursor, joy1cursor, joy2cursor, joy3cursor, joy4cursor;
int pot1_level = 0;
int pot2_level = 0;
int pot3_level = 0;
int pot4_level = 0;
int joy1_level = 0;
int joy2_level = 0;
int joy3_level = 0;
int joy4_level = 0;
int dataSpeed;
String firmwareVersionMaster = "v1.0.2";
String connection; 
String btn1Dir_Type;
String btn2Dir_Type;
String pot1Dir_Type;
String pot2Dir_Type;
String pot3Dir_Type;
String pot4Dir_Type;
String joy1Dir_Type;
String joy2Dir_Type;
String joy3Dir_Type;
String joy4Dir_Type;
String rot1Dir_Type;
String dataSpeed_Type;
String communication_Type;
String radio_Type;
String controlMode_Type;


void oled_display();
void menuControllers();
void memorySaving();
void buttonsAndCotrollers(void *pvParameters);

void potentiometer();
void controller_settings();
void rotary_encoder();
void settings2();
void settings();
void radio_settings();
void button_settings();
void popup_submenu(int col, int row, int width, int height, int radius, int textCol, int textRow, String menuName);
uint16_t adc_controller_cursor_position(uint8_t value, uint8_t directionValue, uint8_t gpioLevel, uint8_t col);


void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  Serial.begin(115200);
  EEPROM.begin(255);

  memorySaving();


  Wire.begin(); // join i2c bus (address optional for master)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(40, 30);
  display.println("Restarted");
  display.display();

  pinMode(nextPageButton, INPUT_PULLUP);
  pinMode(sendDataButton, INPUT_PULLUP);
  pinMode(MainMenuButton, INPUT_PULLUP);
  pinMode(SubMenuButton, INPUT_PULLUP);
  pinMode(Buzzer, OUTPUT);


  xTaskCreatePinnedToCore(
    buttonsAndCotrollers
    ,  "buttonsAndCotrollers"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  1);

  digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);
  delay(100);
  digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);

}



void loop() {

  oled_display();

}


void oled_display(){
connection = (connectionStatus == 0)?"Disconnected":"Connected"; 
btn1Dir_Type = (btn1Dir_value == 0)?"PSH":"TGL";
btn2Dir_Type = (btn2Dir_value == 0)?"PSH":"TGL";

pot1Dir_Type = (pot1Dir_value == 0)?"LFT":"RHT";
pot2Dir_Type = (pot2Dir_value == 0)?"LFT":"RHT";
pot3Dir_Type = (pot3Dir_value == 0)?"LFT":"RHT";
pot4Dir_Type = (pot4Dir_value == 0)?"LFT":"RHT";

joy1Dir_Type = (joy1Dir_value == 0)?"LFT":"RHT";
joy2Dir_Type = (joy2Dir_value == 0)?"LFT":"RHT";
joy3Dir_Type = (joy3Dir_value == 0)?"LFT":"RHT";
joy4Dir_Type = (joy4Dir_value == 0)?"LFT":"RHT";

radio_Type = (radio_value == 0)?"LORA":"NRF24";
//dataSpeed_Type = (dataSpeed_value == 1)?"10":(dataSpeed_value == 2)?"50":(dataSpeed_value == 3)?"100":"500";
dataSpeed_Type = (dataSpeed_value == 1)?"Fast":"Slow";

communication_Type = (communication_value == 1)?"TX":(communication_value == 2)?"RX":"TX&RX";
controlMode_Type = (controlMode_value == 0)?"MODE1":"MODE2";

int changePageButton = digitalRead(nextPageButton);
int changeMenuButton = digitalRead(MainMenuButton);
int changeSubMenuButton = digitalRead(SubMenuButton);
   
   if(MainMenuSelect > 0){//If we are selecting the menu the page change button should not work because the values in the memory will mismatch.
      changePageButton = HIGH;
   }

   if(SubMenuSelect > 0){//If we are selecting the sub-menu the main menu change button should not work because the values in the memory will mismatch.
      changeMenuButton = HIGH;
      changePageButton = HIGH;
   }

   if(pagesCount < 3){//When pages is Not related To "menu select", the "main menu" and "submenu" should not work
      changeMenuButton = HIGH;
      changeSubMenuButton = HIGH;
   }

   if(MainMenuSelect == 0){//When menu value is not selected the "sub menu" value should not work
      changeSubMenuButton = HIGH;    
    }

  
    if (changePageButton == LOW) {
      pagesCount++;
      if (pagesCount > totalPages) { 
        pagesCount = 0;
      }    
        digitalWrite(Buzzer, HIGH);
        delay(100);
        digitalWrite(Buzzer, LOW);
    }

    if (changeMenuButton == LOW) {//Menu select
      MainMenuSelect++;
      if (MainMenuSelect == totalMenus+1) {
        MainMenuSelect = 0;
      }
        digitalWrite(Buzzer, HIGH);
        delay(100);
        digitalWrite(Buzzer, LOW);
     }


    if (changeSubMenuButton == LOW) {//Submenu select
      SubMenuSelect++;
      if (SubMenuSelect == totalSubMenus+1) {
        SubMenuSelect = 0;
      }
        digitalWrite(Buzzer, HIGH);
        delay(100);
        digitalWrite(Buzzer, LOW);
    }
   
   


   
    digitalWrite(Buzzer, LOW);


  

 
  switch(pagesCount){
  
    case 0:
            EEPROM.write(pageCount_address, 0);
            display.clearDisplay();
            display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(3, 1);//COL,ROW
            display.print("DASHBOARD");          
            display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

            display.setTextColor(WHITE);
            display.setTextSize(1);

            display.setCursor(0,12); display.print("RADIO:" + String(radio_Type)+","+String(communication_Type)+","+String(dataSpeed_Type)); 
//            display.setCursor(0,12); display.print("STATUS:" + String(connection)); 
//            display.setCursor(0,21); display.print("TEMP:" + String(tmp));display.setCursor(60,21);display.print("  HUMI:" + String(humi)); 
            display.setCursor(0,21); display.print("RSSI:" + String(rsi));//display.setCursor(60,31);display.print("  SNR:" + String(snr)); 
//            display.setCursor(0,31); display.print("B1:" + String(button1State)+"["+ String(btn1Status)+"]"); display.setCursor(60,31);display.print("| B2:" + String(button2State)+"["+ String(btn2Status)+"]"); 
//            display.setCursor(0,41); display.print("S1:" + String(switch1State)+"["+ String(sw1Status)+"]"); display.setCursor(60,41);display.print("| S2:" + String(switch2State)+"["+ String(sw2Status)+"]"); 
            display.setCursor(0,31); display.print("SWT:" +String(button1State)+","+String(button2State)+","+String(switch1State)+","+ String(switch2State)+" ["+String(btn1Status)+","+String(btn2Status)+","+String(sw1Status)+","+String(sw2Status)+"]");
            

//            display.setCursor(0,12); display.print("cordinates:"+String(latitude)+","+String(longitude));
//            display.setCursor(0,21); display.print("distance  :"+String(distance));
//            display.setCursor(0,31); display.print("voltage   :"+String(voltage));

//            "["+ String(btn1Status)+"]"); display.setCursor(60,31);display.print("| B2:" + String(button2State)+"["+ String(btn2Status)+"]"); 
//            display.setCursor(0,41); display.print("S1:" +String(switch1State)+"["+ String(sw1Status)+"]"); display.setCursor(60,41);display.print("| S2:" + String(switch2State)+"["+ String(sw2Status)+"]");          
          break;
  
    case 1:
            EEPROM.write(pageCount_address, 1);
            display.clearDisplay();
            display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(3, 1);//COL,ROW
            display.println("POTENTIOMETER");
            display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));
            
            display.setTextColor(WHITE);
            display.drawRect(0, 10, 128, 50, WHITE);           
            display.setCursor(3,13); display.print("POT1:"+String(pot1_level));display.setCursor(50,13);display.print(" ["+ String(pot1Status)+"]");
            display.setCursor(3,23); display.print("POT2:"+String(pot2_level));display.setCursor(50,23);display.print(" ["+ String(pot2Status)+"]"); 
            display.setCursor(3,33); display.print("POT3:"+String(pot3_level));display.setCursor(50,33);display.print(" ["+ String(pot3Status)+"]");
            display.setCursor(3,43); display.print("POT4:"+String(pot4_level));display.setCursor(50,43);display.print(" ["+ String(pot4Status)+"]");          
          break;
          
    case 2:
            EEPROM.write(pageCount_address, 2);
            display.clearDisplay();
            display.fillRoundRect(0, 0, 128, 9, 2, WHITE);            
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(3, 1);//COL,ROW
            display.println("JOYSTICKS-"+String(controlMode_Type));
            display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

            display.setTextColor(WHITE);
            display.drawRect(0, 10, 128, 26, WHITE);
//            display.drawRect(63, 33, 65, 31, WHITE);

            display.setCursor(3,15); display.print("JOY1:" + String(joy1_level));display.setCursor(60,15);display.print("| JOY2:" + String(joy2_level)); 
            display.setCursor(3,25); display.print("JOY3:" + String(joy3_level));display.setCursor(60,25);display.print("| JOY4:" + String(joy4_level)); 

            joy1cursor = adc_controller_cursor_position(joy1_value, joy1Dir_value, joy1_level, 120);
            joy2cursor = adc_controller_cursor_position(joy2_value, joy2Dir_value, joy2_level, 120);
            joy3cursor = adc_controller_cursor_position(joy3_value, joy3Dir_value, joy3_level, 120);
            joy4cursor = adc_controller_cursor_position(joy4_value, joy4Dir_value, joy4_level, 120);

//            display.fillRoundRect(0, 40, 62, 3, 0, WHITE);
//            display.setTextColor(BLACK);
//            display.setTextSize(1);
//            display.setCursor(joy1cursor,38); display.print("*");
//            display.setTextColor(WHITE);

            display.fillRoundRect(0, 40, 128, 3, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(joy1cursor,38); display.print("*");
            display.setTextColor(WHITE);

            display.fillRoundRect(0, 47, 128, 3, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(joy2cursor,45); display.print("*");
            
            display.fillRoundRect(0, 54, 128, 3, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(joy3cursor,52); display.print("*");
            display.setTextColor(WHITE);

            display.fillRoundRect(0, 61, 128, 3, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(joy4cursor,59); display.print("*");



    
           break;
    
    case 3:
          EEPROM.write(pageCount_address, 3);
          potentiometer();
//          potentiometer("potentiometer", pot1_value, pot2_value, pot3_value, pot4_value, joy1_value, joy2_value, joy3_value, joy4_value,
//          pot1_address, pot2_address, pot3_address, pot4_address, joy1_address, joy2_address, joy3_address, joy4_address, 
//          180, 255);        
          break;
          
    case 4:
          EEPROM.write(pageCount_address, 4);
          controller_settings();
          break;
    case 5:
          EEPROM.write(pageCount_address, 5);
          button_settings();
          break;      
    case 6:
          EEPROM.write(pageCount_address, 6); 
          settings();
          break;   
    case 7:
          EEPROM.write(pageCount_address, 7);
          settings2();
//          display.clearDisplay();
//          display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
//          display.setTextColor(BLACK);
//          display.setTextSize(1);
//          display.setCursor(3, 1);//COL,ROW
//          display.print("FIRMWARE");          
//          display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));
//  
//          display.setTextColor(WHITE);
//          display.setCursor(0,15); display.print("Master(TX) : " + String(firmwareVersionMaster)); 
//  //      display.setCursor(0,25); display.print("Slave1(RX) : "); display.println(fwVersionSlave1); 
//  //      display.setCursor(0,35); display.print("SLave2(TX) : " + String(fwVersionSlave2));          
//  //      display.setCursor(0,45); display.print(" POT2:" + String(pot2_value) + "  | "); display.setCursor(70,21);display.print(" BTN2:" + String(btn1Dir_value)); 
          break;  
    case 8:
          EEPROM.write(pageCount_address, 8);
   
          break;                    
      case 9:
            EEPROM.write(pageCount_address, 9);
            break;
  
  }
           display.display();

  EEPROM.commit(); 

}







void memorySaving(){
  Serial.println("Memory saved");

  EEPROM.begin(255);
   
  pot1_value = EEPROM.read(pot1_address);//EEPROM.read(ADDRESS 0-255);
  pot2_value = EEPROM.read(pot2_address);//EEPROM.read(ADDRESS 0-255);
  pot3_value = EEPROM.read(pot3_address);//EEPROM.read(ADDRESS 0-255);
  pot4_value = EEPROM.read(pot4_address);//EEPROM.read(ADDRESS 0-255);
  
  joy1_value = EEPROM.read(joy1_address);//EEPROM.read(ADDRESS 0-255);
  joy2_value = EEPROM.read(joy2_address);//EEPROM.read(ADDRESS 0-255);
  joy3_value = EEPROM.read(joy3_address);//EEPROM.read(ADDRESS 0-255);
  joy4_value = EEPROM.read(joy4_address);//EEPROM.read(ADDRESS 0-255);

  pot1Dir_value = EEPROM.read(pot1Dir_address);//EEPROM.read(ADDRESS 0-255);
  pot2Dir_value = EEPROM.read(pot2Dir_address);//EEPROM.read(ADDRESS 0-255);
  pot3Dir_value = EEPROM.read(pot3Dir_address);//EEPROM.read(ADDRESS 0-255);
  pot4Dir_value = EEPROM.read(pot4Dir_address);//EEPROM.read(ADDRESS 0-255);

  joy1Dir_value = EEPROM.read(joy1Dir_address);//EEPROM.read(ADDRESS 0-255);
  joy2Dir_value = EEPROM.read(joy2Dir_address);//EEPROM.read(ADDRESS 0-255);
  joy3Dir_value = EEPROM.read(joy3Dir_address);//EEPROM.read(ADDRESS 0-255);
  joy4Dir_value = EEPROM.read(joy4Dir_address);//EEPROM.read(ADDRESS 0-255);
  
  btn1Dir_value = EEPROM.read(btn1Dir_address);//EEPROM.read(ADDRESS 0-255);
  btn2Dir_value = EEPROM.read(btn2Dir_address);//EEPROM.read(ADDRESS 0-255);


   pagesCount = EEPROM.read(pageCount_address);//EEPROM.read(ADDRESS 0-255); 



  dataSpeed_value = EEPROM.read(dataSpeed_address);//EEPROM.read(ADDRESS 0-255);
  communication_value = EEPROM.read(communication_address);//EEPROM.read(ADDRESS 0-255);
  radio_value = EEPROM.read(radio_address);//EEPROM.read(ADDRESS 0-255);
  controlMode_value = EEPROM.read(controlMode_address);//EEPROM.read(ADDRESS 0-255);


  joy1CalibrateValue = EEPROM.read(joy1CalibrationAddress);
  joy2CalibrateValue = EEPROM.read(joy2CalibrationAddress);
  joy3CalibrateValue = EEPROM.read(joy3CalibrationAddress);
  joy4CalibrateValue = EEPROM.read(joy4CalibrationAddress);
  calibrationButton = EEPROM.read(calibration_address);
  joyOffsetValue = EEPROM.read(joyOffsetAddress);
  Serial.println("Read =  calibrationButton:" +String(calibrationButton)+",joy1CalibrateValue:" + String(joy1CalibrateValue)+",joy2CalibrateValue:" + String(joy2CalibrateValue)+",joy3CalibrateValue:" + String(joy3CalibrateValue)+",joy4CalibrateValue:" + String(joy4CalibrateValue));
  Serial.println("Read =  Control Type:" + String(controlMode_value)); 

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
    pot1_level = potentiometer_controller(pot1_value, pot1Dir_value, potentiometer1);
    pot2_level = potentiometer_controller(pot2_value, pot2Dir_value, potentiometer2);
    pot3_level = potentiometer_controller(pot3_value, pot3Dir_value, potentiometer3);
    pot4_level = potentiometer_controller(pot4_value, pot4Dir_value, potentiometer4);  


    if(controlMode_value == 0){
      joy1_level = joystick_controller(joystick1, joy1_value, joy1Dir_value, joy1CalibrateValue, joyOffsetValue);
      joy4_level = joystick_controller(joystick2, joy4_value, joy4Dir_value, joy4CalibrateValue, joyOffsetValue);
      joy3_level = joystick_controller(joystick3, joy3_value, joy3Dir_value, joy3CalibrateValue, joyOffsetValue);
      joy2_level = joystick_controller(joystick4, joy2_value, joy2Dir_value, joy2CalibrateValue, joyOffsetValue);
    }
    else if(controlMode_value == 1){
      joy3_level = joystick_controller(joystick1, joy3_value, joy3Dir_value, joy3CalibrateValue, joyOffsetValue);
      joy2_level = joystick_controller(joystick2, joy2_value, joy2Dir_value, joy2CalibrateValue, joyOffsetValue);
      joy1_level = joystick_controller(joystick3, joy1_value, joy1Dir_value, joy1CalibrateValue, joyOffsetValue);
      joy4_level = joystick_controller(joystick4, joy4_value, joy4Dir_value, joy4CalibrateValue, joyOffsetValue);
    }
  
    button1State = button_controller(joyButton1, 1, btn1Dir_value);
    button2State = button_controller(joyButton2, 2, btn2Dir_value);
    switch1State = button_controller(switch1, 3, 0);
    switch2State = button_controller(switch2, 4, 0);

if(button1State == 1){ button1State = 0; }
else if(button1State == 0){ button1State = 1; }

if(button2State == 1){ button2State = 0; }
else if(button2State == 0){ button2State = 1; }

if(switch1State == 1){ switch1State = 0; }
else if(switch1State == 0){ switch1State = 1; }

if(switch2State == 1){ switch2State = 0; }
else if(switch2State == 0){ switch2State = 1; }

vTaskDelay(1);
}
}




//*****POPUP SUBMENU**************
void popup_submenu(int col, int row, int width, int height, int radius, int textCol, int textRow, String menuName){
  //fillRect(x, y, width, height, color)
  display.fillRoundRect(col, row, width, height, radius, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(textCol, textRow);//COL,ROW
  display.println(menuName);
}

void potentiometer(){
          display.clearDisplay();
          display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
          display.setTextColor(BLACK);
          display.setTextSize(1);
          display.setCursor(3, 1);//COL,ROW
          display.print("POTENTIOMETER");          
          display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

          display.setTextColor(WHITE);
          display.drawRect(0, 10, 65, 45, WHITE);
          display.drawRect(64, 10, 64, 45, WHITE);

          display.setCursor(8,15); display.print("POT1:" + String(pot1_value)); display.setCursor(73,15);display.print("JOY1:" + String(joy1_value));   
          display.setCursor(8,25); display.print("POT2:" + String(pot2_value)); display.setCursor(73,25);display.print("JOY2:" + String(joy2_value)); 
          display.setCursor(8,35); display.print("POT3:" + String(pot3_value)); display.setCursor(73,35);display.print("JOY3:" + String(joy3_value));   
          display.setCursor(8,45); display.print("POT4:" + String(pot4_value)); display.setCursor(73,45);display.print("JOY4:" + String(joy4_value)); 
                    
            switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,56);display.print("Press To Select Menu");
                  break;       
            case 1:
                  display.fillRoundRect(4, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,15); display.print("POT1:" + String(pot1_value));
                  display.setTextColor(WHITE);
                                                 
                  switch(SubMenuSelect){

                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(pot1_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(pot1_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;                   
            case 2:
                  display.fillRoundRect(4, 24, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,25); display.print("POT2:" + String(pot2_value));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(pot2_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(pot2_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;  
            case 3:
                  display.fillRoundRect(4, 34, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,35); display.print("POT3:" + String(pot3_value));
                  display.setTextColor(WHITE);
                                                 
                  switch(SubMenuSelect){

                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(pot3_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(pot3_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;                   
            case 4:
                  display.fillRoundRect(4, 44, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,45); display.print("POT4:" + String(pot4_value));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(pot4_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(pot4_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break; 
             
             case 5:
                  display.fillRoundRect(69, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,15); display.print("JOY1:" + String(joy1_value));
                  display.setTextColor(WHITE);
                  

                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(25,35);display.print("  180  255");
                                
                                break;
                         case 2:
                                EEPROM.write(joy1_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(joy1_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;                   

             case 6:
                  display.fillRoundRect(69, 24, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,25); display.print("JOY2:" + String(joy2_value));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(joy2_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(joy2_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;      
             case 7:
                  display.fillRoundRect(69, 34, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,35); display.print("JOY3:" + String(joy3_value));
                  display.setTextColor(WHITE);
                  

                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(25,35);display.print("  180  255");
                                
                                break;
                         case 2:
                                EEPROM.write(joy3_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(joy3_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;                   

             case 8:
                  display.fillRoundRect(69, 44, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,45); display.print("JOY4:" + String(joy4_value));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(25,35);display.print("  180  255");
                                break;
                         case 2:
                                EEPROM.write(joy4_address, 180);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(25,35);display.print(" >180  255");
                                break;
                         case 3:
                                EEPROM.write(joy4_address, 255);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(25,35);display.print("  180 >255");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;
              case 9:
                  MainMenuSelect = 0;
                  break;                             
                  }
        display.display(); 
     
}


void button_settings(){
          display.clearDisplay();
          display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
          display.setTextColor(BLACK);
          display.setTextSize(1);
          display.setCursor(3, 1);//COL,ROW
          display.print("SWITCH SETTINGS");          
          display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

          display.setTextColor(WHITE);
          display.drawRect(0, 10, 65, 45, WHITE);
          display.drawRect(64, 10, 64, 45, WHITE);
          display.setCursor(8,15); display.print("BTN1:" + String(btn1Dir_Type)); display.setCursor(73,15);display.print("BTN2:" + String(btn2Dir_Type)); 
  
            switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,56);display.print("Press To Select Menu");
                  break;       

            case 1:
                  display.fillRoundRect(4, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,15); display.print("BTN1:" + String(btn1Dir_Type));
                  display.setTextColor(WHITE);          
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 1");
                                display.setCursor(20,35);display.print("  PUSH  TOGGLE");
                                break;
                         case 2:
                                EEPROM.write(btn1Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 1");
                                display.setCursor(20,35);display.print(" >PUSH  TOGGLE");
                                break;
                         case 3:
                                EEPROM.write(btn1Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 1");
                                display.setCursor(20,35);display.print("  PUSH >TOGGLE");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;   
             
                            
            case 2:
                  display.fillRoundRect(69, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,15); display.print("BTN2:" + String(btn2Dir_Type));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 2");
                                display.setCursor(20,35);display.print("  PUSH  TOGGLE");
                                break;
                         case 2:
                                EEPROM.write(btn2Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 2");
                                display.setCursor(20,35);display.print(" >PUSH  TOGGLE");
                                break;
                         case 3:
                                EEPROM.write(btn2Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 40, 25, "BUTTON 2");
                                display.setCursor(20,35);display.print("  PUSH >TOGGLE");
                                 break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;
             
                 case 3:
                  MainMenuSelect = 0;
                  break;                         
                  }
        display.display(); 
     
}

void controller_settings(){
          display.clearDisplay();
          display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
          display.setTextColor(BLACK);
          display.setTextSize(1);
          display.setCursor(3, 1);//COL,ROW
          display.print("CONTROL SETTINGS");          
          display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

          display.setTextColor(WHITE);
          display.drawRect(0, 10, 65, 45, WHITE);
          display.drawRect(64, 10, 64, 45, WHITE);
          
          display.setCursor(8,15); display.print("POT1:" + String(pot1Dir_Type)); display.setCursor(73,15);display.print("JOY1:" + String(joy1Dir_Type));   
          display.setCursor(8,25); display.print("POT2:" + String(pot2Dir_Type)); display.setCursor(73,25);display.print("JOY2:" + String(joy2Dir_Type)); 
          display.setCursor(8,35); display.print("POT3:" + String(pot3Dir_Type)); display.setCursor(73,35);display.print("JOY3:" + String(joy3Dir_Type));   
          display.setCursor(8,45); display.print("POT4:" + String(pot4Dir_Type)); display.setCursor(73,45);display.print("JOY4:" + String(joy4Dir_Type)); 
  
            switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,56);display.print("Press To Select Menu");
                  break;       
            case 1:
                  display.fillRoundRect(4, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,15); display.print("POT1:" + String(pot1Dir_Type));
                  display.setTextColor(WHITE);
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(pot1Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(pot1Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 1");                            
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;                   
            case 2:
                  display.fillRoundRect(4, 24, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,25); display.print("POT2:" + String(pot2Dir_Type));
                  display.setTextColor(WHITE);
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(pot2Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(pot2Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 2");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;    
            case 3:
                  display.fillRoundRect(4, 34, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,35); display.print("POT3:" + String(pot3Dir_Type));
                  display.setTextColor(WHITE);
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(pot3Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(pot3Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 3");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;    
            case 4:
                  display.fillRoundRect(4, 44, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(8,45); display.print("POT4:" + String(pot4Dir_Type));
                  display.setTextColor(WHITE);
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(pot4Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(pot4Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 22, 25, "POTENTIOMETER 4");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;    
             

             case 5:
                  display.fillRoundRect(69, 14, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,15); display.print("JOY1:" + String(joy1Dir_Type));
                  display.setTextColor(WHITE);
             
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(joy1Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(joy1Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 1");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;   
            case 6:
                  display.fillRoundRect(69, 24, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,25); display.print("JOY2:" + String(joy2Dir_Type));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(joy2Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(joy2Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 2");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;  
                
            case 7:
                  display.fillRoundRect(69, 34, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,35); display.print("JOY3:" + String(joy3Dir_Type));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(joy3Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(joy3Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 3");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;  
            case 8:
                  display.fillRoundRect(69, 44, 55, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(73,45); display.print("JOY4:" + String(joy4Dir_Type));
                  display.setTextColor(WHITE);
                  
                  switch(SubMenuSelect){
                         case 1:
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(20,35);display.print("  LEFT  RIGHT");
                                break;
                         case 2:
                                EEPROM.write(joy4Dir_address, 0);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(20,35);display.print(" >LEFT  RIGHT");
                                break;
                         case 3:
                                EEPROM.write(joy4Dir_address, 1);
                                popup_submenu(10, 20, 108, 25, 5, 33, 25, "JOYSTICK 4");
                                display.setCursor(20,35);display.print("  LEFT >RIGHT");
                                break;     
                         case 4:
                              SubMenuSelect = 0;  
                                break;
                  }  
                  break;               
              case 9:
                  MainMenuSelect = 0;
                  break;                    
                  }
        display.display(); 
     
}


void page_settings(String pageName, String menu1, String menu2, String menu3, String menu4, String btn1Dir, String btn2Dir, String btn3Dir, 
String btn4Dir, String btnType1, String btnType2, int addr1, int addr2, int addr3, int addr4, int addrValue1, int addrValue2){
          display.clearDisplay();
          display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
          display.setTextColor(BLACK);
          display.setTextSize(1);
          display.setCursor(3, 1);//COL,ROW
          display.print(pageName);          
          display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));

          display.setTextColor(WHITE);        
          display.setCursor(0,12);display.print(" "+String(menu1) + ":" + String(btn1Dir));
          display.setCursor(0,21);display.print(" "+String(menu2) + ":" + String(btn2Dir));   
          display.setCursor(0,31);display.print(" "+String(menu3) + ":" + String(btn3Dir));
          display.setCursor(0,41);display.print(" "+String(menu4) + ":" + String(btn4Dir)); 

           switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,55);display.print("Please Select Menu");
                  break;          
              
          case 1:
                 display.setCursor(0,12); display.print(">"+String(menu1)+ ":" + String(btn1Dir));
                 switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 1:
                            EEPROM.write(addr1, addrValue1);
                            display.setCursor(10,55);display.print(" >"+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 2:
                            EEPROM.write(addr1, addrValue2);
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print(" >"+String(btnType2));
                            break;
                     case 3:
                           SubMenuSelect = 0;
                           break;           
              }  
              break;                   
        case 2:
              display.setCursor(0,21); display.print(">"+String(menu2)+ ":" + String(btn2Dir));
              switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 1:
                            EEPROM.write(addr2, addrValue1);
                            display.setCursor(10,55);display.print(" >"+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 2:
                            EEPROM.write(addr2, addrValue2);
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print(" >"+String(btnType2));
                            break;     
                     case 3:
                           SubMenuSelect = 0;
                           break; 
              }   
              break;  
        case 3:
              display.setCursor(0,31); display.print(">"+String(menu3)+ ":" + String(btn3Dir));
              switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 1:
                            EEPROM.write(addr3, addrValue1);
                            display.setCursor(10,55);display.print(" >"+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 2:
                            EEPROM.write(addr3, addrValue2);
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print(" >"+String(btnType2));
                            break;     
                     case 3:
                           SubMenuSelect = 0;
                           break; 
              }  
              break;                    
        case 4:
              display.setCursor(0,41); display.print(">"+String(menu4)+ ":" + String(btn4Dir));
              switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 1:
                            EEPROM.write(addr4, addrValue1);
                            display.setCursor(10,55);display.print(" >"+String(btnType1));display.setCursor(55,55);display.print("  "+String(btnType2));
                            break;
                     case 2:
                            EEPROM.write(addr4, addrValue2);
                            display.setCursor(10,55);display.print("  "+String(btnType1));display.setCursor(55,55);display.print(" >"+String(btnType2));
                            break;     
                     case 3:
                           SubMenuSelect = 0;
                           break; 
              }   
              break;                    
                       
         case 5:
                  MainMenuSelect = 0;
                  break;   
              } 
        display.display();        
}





void settings(){
          display.clearDisplay();
            display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(3, 1);//COL,ROW
            display.print("SETTINGS");          
            display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));


          display.setTextColor(WHITE);
          display.drawRect(0, 10, 128, 45, WHITE);
          
          display.setCursor(3,12); display.print("Transmit Speed:" + String(dataSpeed_Type));
          display.setCursor(3,22); display.print("Communication :" + String(communication_Type));    
          display.setCursor(3,32); display.print("Radio Type    :" + String(radio_Type));
          display.setCursor(3,42); display.print("Factory Reset");     

          
           switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,55);display.print("Select Transmit Speed");
                  break;          
              
          case 1:
                  display.fillRoundRect(0, 11, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,12); display.print("Transmit Speed:" + String(dataSpeed_Type));
                  display.setTextColor(WHITE);
                  
                 
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 22, 25, "Transmit Speed");
                            display.setCursor(20,35);display.print(" Fast  Slow");
                            break;
                     case 2:
                            EEPROM.write(dataSpeed_address, 1);
                            popup_submenu(10, 20, 108, 25, 5, 22, 25, "Transmit Speed");
                            display.setCursor(20,35);display.print(">Fast  Slow");
                            break;
                     case 3:
                            EEPROM.write(dataSpeed_address, 2);
                            popup_submenu(10, 20, 108, 25, 5, 22, 25, "Transmit Speed");
                            display.setCursor(20,35);display.print(" Fast >Slow");
                            break;      
                     case 4:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break; 
          case 2:
                  display.fillRoundRect(0, 21, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,22); display.print("Communication :" + String(communication_Type)); 
                  display.setTextColor(WHITE);
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Communication");
                            display.setCursor(20,35);display.print(" TX  RX  TX_RX");
                            break;
                     case 2:
                            EEPROM.write(communication_address, 1);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Communication");
                            display.setCursor(20,35);display.print(">TX  RX  TX_RX");
                            break;
                     case 3:
                            EEPROM.write(communication_address, 2);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Communication");
                            display.setCursor(20,35);display.print(" TX >RX  TX_RX");
                            break;     
                     case 4:
                            EEPROM.write(communication_address, 3);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Communication");
                            display.setCursor(20,35);display.print(" TX  RX >TX_RX");
                            break;
                     case 5:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;                                 
          case 3:
                  display.fillRoundRect(0, 31, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,32); display.print("Radio Type    :" + String(radio_Type)); 
                  display.setTextColor(WHITE);
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Radio Type");
                            display.setCursor(20,35);display.print(" LORA  NRF24L01");
                            break;
                     case 2:
                            EEPROM.write(radio_address, 0);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Radio Type");
                            display.setCursor(20,35);display.print(">LORA  NRF24L01");
                            break;
                     case 3:
                            EEPROM.write(radio_address, 1);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Radio Type");
                            display.setCursor(20,35);display.print(" LORA >NRF24L01");
                            break;     
                     case 4:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;

          case 4:
                  display.fillRoundRect(0, 41, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,42); display.print("Factory Reset"); 
                  display.setTextColor(WHITE);
                  
                 
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Factory Reset");
                            display.setCursor(40,35);display.print(" YES  NO");
                            break;
                     case 2:
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Factory Reset");
                            display.setCursor(40,35);display.print(">YES  NO");
                            EEPROM.write(pot1_address, 180);
                            EEPROM.write(pot2_address, 180);
                            EEPROM.write(pot3_address, 180);
                            EEPROM.write(pot4_address, 180);                            
                            EEPROM.write(joy1_address, 255);
                            EEPROM.write(joy2_address, 255);
                            EEPROM.write(joy3_address, 255);
                            EEPROM.write(joy4_address, 255);
                            
                            break;
                     case 3:
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Factory Reset");
                            display.setCursor(40,35);display.print(" YES >NO");
                            break;     
                     case 4:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;
                                               
         case 5:
                  MainMenuSelect = 0;
                  break;   
              }  
       display.display();        
}


void settings2(){
          display.clearDisplay();
            display.fillRoundRect(0, 0, 128, 9, 2, WHITE);
            display.setTextColor(BLACK);
            display.setTextSize(1);
            display.setCursor(3, 1);//COL,ROW
            display.print("SETTINGS2");          
            display.setCursor(103,1);display.print(String(pagesCount) + "/" + String(totalPages));


          display.setTextColor(WHITE);
          display.drawRect(0, 10, 128, 45, WHITE);
          display.setCursor(3,12); display.print("Master(TX)  : " + String(firmwareVersionMaster));           
          display.setCursor(3,22); display.print("Control Type:" + String(controlMode_Type));
          display.setCursor(3,32); display.print("Joystick Calibration");
          display.setCursor(3,42); display.print("Joystick Offset :" + String(joyOffsetValue));
          

              if(calibrationButton == 1){
      int i = 0;
      while (i <= 100) { //2seconds
        Serial.println("Calibrating Joysticks for 2 sec: " + String(i));
        display.clearDisplay(); 
        display.setTextColor(WHITE);
        display.setCursor(0,30); display.print("Calibrating...");display.print(i); 
        display.display();
          
        i++;
          
        if (i == 100) {
         
     joystick_calibration(joystick1, joy1_value, joy1Dir_value, joy1CalibrationAddress);
     joystick_calibration(joystick2, joy2_value, joy2Dir_value, joy2CalibrationAddress);
     joystick_calibration(joystick3, joy3_value, joy3Dir_value, joy3CalibrationAddress);
     joystick_calibration(joystick4, joy4_value, joy4Dir_value, joy4CalibrationAddress);      
          
          delay(1000);
          display.clearDisplay();
          display.setTextSize(2);
          display.setCursor(35,30); display.print("DONE");
          display.display(); 
          EEPROM.write(calibration_address, 0);
          EEPROM.commit();
          resetFunc();  //call reset
          

        }
    }
}
           switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,55);display.print("Select Menu");
                  break;          
                                          
            case 1:
                  display.fillRoundRect(0, 21, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,22); display.print("Control Type:" + String(controlMode_Type));
                  display.setTextColor(WHITE);
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Control Type");
                            display.setCursor(20,35);display.print(" MODE1  MODE2");
                            break;
                     case 2:
                            EEPROM.write(controlMode_address, 0);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Control Type");
                            display.setCursor(20,35);display.print(">MODE1  MODE2");
                            break;
                     case 3:
                            EEPROM.write(controlMode_address, 1);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Control Type");
                            display.setCursor(20,35);display.print(" MODE1 >MODE2");
                            break;     
                     case 4:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;

            case 2:
                  display.fillRoundRect(0, 31, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,32); display.print("Joystick Calibration");
                  display.setTextColor(WHITE);
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Calibration");
                            display.setCursor(40,35);display.print(" YES  NO");
                            break;
                     case 2:
                            EEPROM.write(calibration_address, 1);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Calibration");
                            display.setCursor(40,35);display.print(">YES  NO");
                            break;
                     case 3:
                            EEPROM.write(calibration_address, 0);
                            popup_submenu(10, 20, 108, 25, 5, 30, 25, "Calibration");
                            display.setCursor(40,35);display.print(" YES >NO");
                            break;     
                     case 4:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;
              
          case 3:
                  display.fillRoundRect(0, 41, 128, 9, 1, WHITE);
                  display.setTextColor(BLACK);
                  display.setCursor(3,42); display.print("Joystick Offset :" + String(joyOffsetValue)); 
                  display.setTextColor(WHITE);
                 switch(SubMenuSelect){
                     case 1:
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Joystick Offset");
                            display.setCursor(40,35);display.print(" 2  4  6");
                            break;
                     case 2:
                            EEPROM.write(joyOffsetAddress, 2);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Joystick Offset");
                            display.setCursor(40,35);display.print(">2  4  6");
                            break;
                     case 3:
                            EEPROM.write(joyOffsetAddress, 4);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Joystick Offset");
                            display.setCursor(40,35);display.print(" 2 >4  6");
                            break;     
                     case 4:
                            EEPROM.write(joyOffsetAddress, 6);
                            popup_submenu(10, 20, 108, 25, 5, 25, 25, "Joystick Offset");
                            display.setCursor(40,35);display.print(" 2  4 >6");
                            break;
                     case 5:
                           SubMenuSelect = 0;
                           break; 
                   }  
              break;
              
            case 4:
                  MainMenuSelect = 0;
                  break;   
              }  
       display.display();        
}






/************POTENTIOMETERS & JOYSTICKS CURSOR POSITION CONTROLLER*********************************/
uint16_t adc_controller_cursor_position(uint8_t value, uint8_t directionValue, uint8_t gpioLevel, uint8_t col){
    uint8_t cursorPosition; 
       if(value == 180 & directionValue == 0){ cursorPosition = map(gpioLevel, 180, 0, 0, col);}
  else if(value == 180 & directionValue == 1){ cursorPosition = map(gpioLevel, 180, 0, col, 0);}
  else if(value == 255 & directionValue == 0){ cursorPosition = map(gpioLevel, 255, 0, 0, col);}
  else if(value == 255 & directionValue == 1){ cursorPosition = map(gpioLevel, 255, 0, col, 0);}
return cursorPosition;
}
