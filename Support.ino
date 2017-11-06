//*********************************************************************************
//***SUPPORT METHODS***************************************************************
//*********************************************************************************

void initializeServer() {
  /*
   * Initializes ESP8266 as a web server
   * This method is used only once in the microcontroller setup()
   */
  //Print initial status
  Serial.println("===================");
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP_STA);//Access Point and Station
  //Create a semi-unique SSID by adding the last 2 bytes of its MAC address
  global_AP_SSID = "ESP-" + WiFi.macAddress().substring(12, 14) + WiFi.macAddress().substring(15);
  const char *AP_SSID = global_AP_SSID.c_str();
  WiFi.softAP(AP_SSID); //Create Access Point without password
  //WiFi.softAP(AP_SSID, "123456"); //Create Access Point with password
  Serial.print("Access Point ID\t: ");
  Serial.println(AP_SSID);
  Serial.print("Access Point IP\t: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("WiFi Connection\t: ");
  delay(3000);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.SSID());
    Serial.print("Local IP\t: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("None");
  }
  //Start server
  Serial.println("Starting HTTP server...");
  //setup http servers
  server.on("/", requestDefault);
  server.on("/stat", requestStatus);
  server.on("/scan", requestScan);
  server.on("/conn", requestConnect);
  server.on("/disc", requestDisconnect);
  server.begin();
  Serial.println("HTTP server ready");
  Serial.println("===================");
}

int scanWifi(String networks[]) {
  /*
   * Scans for nearby WiFi networks
   * Adds unique SSIDs to networks[]
   * Returns number of SSIDs added
   */
  int n = WiFi.scanNetworks();
  int j = 0;
  String ssid;
  if (n != 0) {
    for (int i = 0; i < 50; i++) { //50 is the original size of the array
      networks[i] = ""; //create new list
    }
    for (int i = 0; i < n; ++i) {
      ssid = WiFi.SSID(i);
      delay(10);
      networks[j++] = ssid;
    }
  }
  return j;
}

String timeStamp(String str, boolean serialComm) {
  /*
   * returns a string in the following format:
   * [elapsedTime][SERIAL or WIFI] [str]
   */
  int h, m, s, ms;
  unsigned long over, elapsed;
  String elapsedTime = "";
  elapsed = millis();

  h = int(elapsed / 3600000);
  if (h < 10) str += "0" + (String)h;
  else elapsedTime += h;
  elapsedTime += ":";

  over = elapsed % 3600000;
  m = int(over / 60000);
  if (m < 10) elapsedTime += "0" + (String)m;
  else elapsedTime += m;
  elapsedTime += ":";

  over = over % 60000;
  s = int(over / 1000);
  if (s < 10) elapsedTime += "0" + (String)s;
  else elapsedTime += s;
  elapsedTime += ".";

  ms = over % 1000;
  if (ms < 10) elapsedTime += "00" + (String)ms;
  else if (ms < 100) elapsedTime += "0" + (String)ms;
  else elapsedTime += (String)ms;
  
  String response = elapsedTime;
  if (serialComm) {
    response += "[SERIAL] ";
  }
  else {
    response += "[WIFI] ";
  }
  response += str;
  return response;
}

IPAddress parseIP(String string) {
  const char* str = string.c_str();
  IPAddress addr;
  for (int i = 0; i < 4; i++) {
    addr[i] = strtoul(str, NULL, 10);
    str = strchr(str, '.');
    if (str == NULL || *str == '0') {
      break; // No more separators, exit
    }
    str++;
  }
  return addr;
}
