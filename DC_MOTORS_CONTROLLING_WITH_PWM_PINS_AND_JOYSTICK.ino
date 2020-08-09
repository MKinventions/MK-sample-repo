int LMT1 = 5; //LEFT MOTOR TERMINAL 1
int LMT2 = 6; //LEFT MOTOR TERMINAL 2
int RMT1 = 9; //RIGHT MOTOR TERMINAL 1
int RMT2 = 10; //RIGHT MOTOR TERMINAL 2

//5,6,9,10 ARE PWM PINS

void setup() {
pinMode(LMT1, OUTPUT);
pinMode(LMT2, OUTPUT);
pinMode(RMT1, OUTPUT);
pinMode(RMT2, OUTPUT);

pinMode(A0, INPUT);//JOYSTICK
}

void loop() {
int JOYSTICK = analogRead(A0);
int POT_VALUE1 = map(JOYSTICK, 0, 1023, 0, 225);//ROTATE CLOCK WISE VALUE FROM 0 TO 1023 WITH SPEED 0 TO 255
int POT_VALUE2 = map(JOYSTICK, 1023, 0, 0, 225);//ROTATE ANTI CLOCK WISE VALUE FROM 1023 TO 0 WITH SPEED 0 TO 255


if(JOYSTICK > 520){  
//SPEED FROM 0 TO 255
// MOTORS ROTATE CLOCKWISE WITH MAXIMUM SPEED 255 
analogWrite(LMT1, POT_VALUE1);
analogWrite(LMT2, 0);
analogWrite(RMT1, POT_VALUE1);
analogWrite(RMT2, 0);
}
else if(JOYSTICK < 500){
// MOTORS ROTATE ANTI-CLOCKWISE WITH MAXIMUM SPEED 255 
analogWrite(LMT1, 0);
analogWrite(LMT2, POT_VALUE2);
analogWrite(RMT1, 0);
analogWrite(RMT2, POT_VALUE2);
}
else{
// MOTORS STOP ROTATE
analogWrite(LMT1, 0);
analogWrite(LMT2, 0);
analogWrite(RMT1, 0);
analogWrite(RMT2, 0);
}
}
