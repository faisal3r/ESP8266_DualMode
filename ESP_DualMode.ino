#include <Wire.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

//Global variables
String global_AP_SSID;
ESP8266WebServer server(80);                  //http server
boolean serialComm;                           //true if current communication is serial (false if HTTP)

void setup(){
  Wire.begin();
  Serial.begin(115200);
  initializeServer();   //Initialize web server for HTTP communications

  //Power indicator
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW); //LOW lights it up!
}

void loop(){
  String inst = ""; //instruction received by serial
  
  if(serialComm = Serial.available()){
    inst = Serial.readString();
    Serial.println(inst);
  }

  //Hadnle received instruction
  if(inst == "stat"){
    requestStatus();
  }else if(inst == "scan"){
    requestScan();
  }else if(inst == "conn"){
    requestConnect();
  }else if(inst == "disc"){
    requestDisconnect();
  }else if(inst != ""){
    Serial.println(timeStamp("Incorrect instruction received: "+inst, serialComm));
  }

  server.handleClient();
}
