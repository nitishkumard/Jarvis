/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

//IR RX/TX
IRsend irsend;
IRrecv irrecv(11);
decode_results results;
unsigned long codeValue;
int codeLen = 12;
int toggle = 1;

//PIR Sensor
int calibrationTime = 1; //ideal is 10
long unsigned int lowIn;
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int pirPin = 2;
int ledPin = 13;

String inputString = "";
boolean stringComplete = false;

void setup()
{
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //PIR Caliberation Time
  for (int i = 0; i < calibrationTime; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  irrecv.enableIRIn();
  //send pi ready signal(optional)
  Serial.println("pu");
}

void sendReadySignal()
{
  //Light Toggle to indicate SystemBoot
  codeValue = 0xB62;
  sendByIR(codeValue);
  delay(500);
  codeValue = 0xB62;
  sendByIR(codeValue);
}

void loop()
{
  serialEvent();
  activateIRSensor();
  activateMotionSensor();
}

void activateIRSensor()
{
  if (irrecv.decode(&results))
  {
    Serial.print("cRecieve:"); Serial.println(results.value, HEX);
    if (results.value == 0xFE18E7)
    { //Shut down pi
      Serial.println("ps");
    }
    else if (results.value == 0xFE10EF)
    { //Reboot pi
      Serial.println("pr");
    }
    else if (results.value == 0xFE40BF)
    { //FanUp
      codeValue = 0xB5E;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFE50AF)
    { //FanDown
      codeValue = 0xB4F;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFE609F)
    { //FanSwitch
      codeValue = 0xB69;
      //sendByIR(codeValue);
      codeValue = 0x64800004;
      sendByIR(codeValue);
      codeValue = 0xFFFFFFFF;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFEE817)
    { //MainLight
      codeValue = 0xB62;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFE6897)
    { //DressingLight
      codeValue = 0x351;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFEA857)
    { //plug
      codeValue = 0xB6B;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFE807F)
    { //Master
      codeValue = 0xB4C;
      sendByIR(codeValue);
    }
    else if (results.value == 0xFEA05F)
    { //Speaker Volume up
      Serial.println("svu");
    }
    else if (results.value == 0xFEE01F)
    { //Speaker Volume down
      Serial.println("svd");
    }
    else if (results.value == 0xFE00FF)
    { //Speaker Volume mute
      Serial.println("svm");
    }
    else if (results.value == 0xFE20DF)
    { //Speaker Channel Rotate
      Serial.println("scr");
    }

    irrecv.enableIRIn();
  }
}

void processSerialData()
{
  if(inputString == "svu")
  {
     
  }
}

void sendByIR(unsigned long codeValue)
{
  delay(100);
  for (int i = 0; i < 1; i++) {
    Serial.print("cSending :");
    Serial.println(codeValue, HEX);
    irsend.sendRC5(codeValue, 12); // Sony TV power code
    //delay(40);
    //codeValue = codeValue & ~(1 << (codeLen - 1));
    //codeValue = codeValue | (toggle << (codeLen - 1));
    //irsend.sendRC5(codeValue, 12);
    //Serial.println(codeValue, HEX);
  }
}

void activateMotionSensor()
{
  if (digitalRead(pirPin) == HIGH)
  {
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    if (lockLow)
    {
      lockLow = false;
    }
    takeLowTime = true;
  }

  if (digitalRead(pirPin) == LOW)
  {
    digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

    if (takeLowTime)
    {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause)
    {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
    }
  }
}


void serialEvent() 
{
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();

    if (inChar != '\r' && inChar != '\n') 
    {
      inputString += inChar;
    }
    if (inChar == '\n') 
    {
      stringComplete = true;
    }
  }
  // print the string when a newline arrives:
  if (stringComplete) 
  {
    processSerialData();
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}



