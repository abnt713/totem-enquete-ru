
#include <SPI.h>
#include <Ethernet.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

char server[] = "192.168.0.2";
IPAddress ip(192,168,0,177);

EthernetClient client;

/* Setting up the buttons */
int buttonPin1 = 2;
int buttonPin2 = 5;
int buttonPin3 = 8;

/* Checking out the states */
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

/* The pressing state - We don't want to process an already pressed button */
int buttonAlreadyPressed1 = 0;
int buttonAlreadyPressed2 = 0;
int buttonAlreadyPressed3 = 0;

/* A LED for feedback */
const int ledPin =  9;
const int TOTAL_BUTTONS = 3;

/* Labeling the buttons */
const int DISLIKE_BUTTON = 0;
const int NEUTRAL_BUTTON = 1;
const int LIKE_BUTTON = 2;

/* If there is an action in course */
boolean sendingAction = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("Ethernet...OK");
}

void loop(){
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  // read the state of the pushbutton value:
  checkAction();
  readButtonStates();
  checkForPressedButtons();
}

void readButtonStates(){
  /* I know! Arrays should be way better, but I tryed and that didn't work! */
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
}

void checkForPressedButtons(){
  
  if(!sendingAction && buttonState1 == 1 && buttonAlreadyPressed1 == 0){
    buttonAlreadyPressed1 = 1;
    sendingAction = true;
    sendAction(DISLIKE_BUTTON);
  }else if(buttonState1 == 0 && buttonAlreadyPressed1 == 1){
    buttonAlreadyPressed1 = 0;
  }
  
  if(!sendingAction && buttonState2 == 1 && buttonAlreadyPressed2 == 0){
    buttonAlreadyPressed2 = 1;
    sendingAction = true;
    sendAction(NEUTRAL_BUTTON);
  }else if(buttonState2 == 0 && buttonAlreadyPressed2 == 1){
    buttonAlreadyPressed2 = 0;
  }
  
  if(!sendingAction && buttonState3 == 1 && buttonAlreadyPressed3 == 0){
    buttonAlreadyPressed3 = 1;
    sendingAction = true;
    sendAction(LIKE_BUTTON);
  }else if(buttonState3 == 0 && buttonAlreadyPressed3 == 1){
    buttonAlreadyPressed3 = 0;
  }
}

void checkAction(){
  if(sendingAction){
    sendingAction = false;
  }
}

void sendAction(int button){
  String data = "";
  switch(button){
    case DISLIKE_BUTTON:
//      Serial.println('D');
    data+="like=0&card=0&comment=Generated_by_totem";
    sendRequestForVote(data);
    break;
    case NEUTRAL_BUTTON:
//      Serial.println('N');
    data+="like=2&card=0&comment=Generated_by_totem";
    sendRequestForVote(data);
    break;
    case LIKE_BUTTON:
    data+="like=4&card=0&comment=Generated_by_totem";
    sendRequestForVote(data);
//      Serial.println('L');
    break;
  }
  
  blinkLed();
}

void blinkLed(){
  int blinkTime = 100;
  int blinks = 3;
  for(int a = 0; a < blinks; a++){
    digitalWrite(ledPin, HIGH);
    delay(blinkTime);
    digitalWrite(ledPin, LOW);
    delay(blinkTime);
  }
  
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
}

void sendRequestForVote(String data){
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  if (client.connect(server, 5000)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("POST /api/v1/enquetes/active HTTP/1.1");
    client.println("Host: 192.168.0.2");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
    
    Serial.println("POST /api/v1/enquetes/active HTTP/1.1");
    Serial.println("Host: 192.168.0.2");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("Connection: close");
    Serial.print("Content-Length: ");
    Serial.println(data.length());
    Serial.println();
    Serial.print(data);
    Serial.println();
    
    Serial.println("Voted");
  } 
  else {
    Serial.println("connection failed");
  }
}
