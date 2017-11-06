//*********************************************************************************
//***REQUEST METHODS***************************************************************
//*********************************************************************************

void requestDefault(){
  /*
   * responds with possible instructions to communicate with this device
   */
  //Create response
  String response = "==================================\n";
  response+="|| WELCOME TO ESP8266 Dual Mode ||\n";
  response+="==================================\n\n";
  response+="To communicate with this ESP device, type the following commands after the url:\n";
  response+="/stat\t\t\tto get current status of the device\n";
  response+="/scan\t\t\tto get available WiFi networks that this device can see\n";
  response+="/conn?data=XXX,YYY\tto make this device connect to SSID XXX using password YYY\n";
  response+="/disc\t\t\tto make this device disconnect from whatever WiFi it is connected to\n";
  
  //Communicate response
  if(!serialComm){
    server.send(200, "text/plain", response);
  }
}

void requestStatus(){
  /*
   * Responds with connected WiFi(s) and IP addresses
   */
  //Create response
  String response = ((String)global_AP_SSID)+"," + WiFi.softAPIP().toString();
  if(WiFi.status() == WL_CONNECTED){
    response += "," + WiFi.SSID() + "," + WiFi.localIP().toString();
  }
  response += "," + WiFi.macAddress();

  //Communicate response
  Serial.println(timeStamp(response,serialComm));
  if(!serialComm){
    server.send(200, "text/plain", response);
  }
}

void requestScan(){
  /*
   * Responds with list of available WiFi networks
   */
  //Create response
  String response = "";
  String networks[50];
  int numberOfNetworks = scanWifi(networks);
  if(numberOfNetworks>0){
    for(int i=0; i<numberOfNetworks; i++){
      response+= networks[i] + ",";
    }
  }
  else{
    response = "No networks found!";
  }

  //Communicate response
  Serial.println(timeStamp(response,serialComm));
  if(!serialComm){
    server.send(200, "text/plain", response);
  }
}

void requestConnect(){
  /*
   * Connects to a WiFi network
   * Responds with "Connected" or "Failed"
   */
  //Create response
  String response;
  String ssid;
  String password;
  boolean attemptConnection = false;
  
  if(serialComm){ //if requested through Serial
    Serial.println("Enter network SSID: ");
    Serial.flush();
    while(Serial.available()==0){} //wait for SSID from Serial
    ssid = Serial.readString();
    delay(10);
    Serial.println(ssid);
    if(true){ //if SSID requires password (assume always for now)
      Serial.println("Enter password: ");
      Serial.flush();
      while(Serial.available()==0){} //wait for password from Serial
      password = Serial.readString();
      delay(10);
      Serial.println(password);
    }
    attemptConnection = true;
  }
  else{ //if requested through HTTP
    if(server.arg("data") != ""){ //check if client provided credentials
      String httpArgs = server.arg("data");
      for(int i=0; i<httpArgs.length(); i++){ //divide received args into SSID and password
        if(httpArgs[i] == ','){
          ssid = httpArgs.substring(0,i);
          password = httpArgs.substring(i+1,httpArgs.length());
        }
      }
      attemptConnection = true;
    }
    else{
      attemptConnection = false;
      response = "Error: No credentials provided!";
    }
  }
  
  if(attemptConnection){
    //check if already connected to this WiFi
    if(WiFi.SSID()==ssid && WiFi.status()==WL_CONNECTED){
      response = "Error: Already connected to ("+ssid+")!";
    }
    else{
      Serial.println(timeStamp("Attempting connection to: ("+ssid+")", serialComm));
      int status = WiFi.begin(ssid.c_str(), password.c_str());
      unsigned long startTime = millis();
      while(WiFi.status() == status){
        Serial.print(".");
        delay(500);
        if(millis()-startTime >= 10000){
          break;
        }
      }
      Serial.println();
      if(WiFi.status() == WL_CONNECTED){
        response = "Connected";
      }
      else{
        response = "Failed";
      }
    }
  }
  
  //Communicate response
  Serial.println(timeStamp(response,serialComm));
  if(!serialComm){
    server.send(200, "text/plain", response);
  }
}

void requestDisconnect(){
  /*
   * Disconnects from current WiFi
   */
  //Create response
  String response = "Disconnected"; 
    
  //Communicate response
  Serial.println(timeStamp(response,serialComm));
  if(!serialComm){
    server.send(200, "text/plain", response);
  }

  delay(1000);
  WiFi.disconnect();
}
