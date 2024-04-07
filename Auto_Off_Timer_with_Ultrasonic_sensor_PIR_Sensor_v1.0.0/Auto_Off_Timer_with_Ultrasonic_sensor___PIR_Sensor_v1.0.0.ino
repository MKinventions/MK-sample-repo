
int echoPin = A1;
int trigPin = A2;


int pir = A0;
int led = 3;
int led2 = 12;
int relay = 2;
//int timer = 3*60; // 1 = 1sec, 60 = 60 sec
int timer = 10; // 1 = 1sec, 60 = 60 sec

int counter = 0;
int i = 0;

long distance;
long distanceInch;
long duration;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int pirSensor = 0;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(pir, INPUT_PULLUP);
  
  Serial.println("restarted");

  
  digitalWrite(led, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(relay, LOW);
}

void loop() {

  pirSensor = digitalRead(pir);

if(pirSensor == 1){
  digitalWrite(led2, HIGH);
}else{
  digitalWrite(led2, LOW);
}

  ULTRASONIC();
  Serial.println("DISTANCE CM:" + String(distance) + ", PIR:" + String(pirSensor));


  if (distance < 15 && pirSensor == 1) {   //50CM if object detectes enter into the condition

    digitalWrite(led, HIGH);
    digitalWrite(led2, pirSensor);
    digitalWrite(relay, HIGH);

    while (1) {      //if object is left then goto while loop
      ULTRASONIC();  // recall the function for checking the distance
      if (distance  >  20) {
        Serial.println("NOT DETECTED:");

        while (counter <= timer) {
          counter++;
          Serial.println("PIR:[" + String(pirSensor) + "] & Ultrasonic:[" + String(distance) + "]] Sensors Detected, Timer:" + String(counter));
          digitalWrite(led, HIGH);
          digitalWrite(relay, HIGH);

          ULTRASONIC(); // recall the function to stop the timer
          if (distance < 20) {
            counter = 0;
            digitalWrite(led, HIGH);
            digitalWrite(relay, HIGH);
          }

        delay(1000);
        }

//        if (counter == timer + 1) {
        if (counter >= timer) {  
          digitalWrite(led, LOW);
          digitalWrite(relay, LOW);
          delay(1000);
          resetFunc();  //call reset
        }

      }
    }
  }




}


void ULTRASONIC() {
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  distanceInch = duration * 0.0133 / 2;

}
