/*
 * HTTPS Secured Client GET Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
const char *ssid = "ATT2GrT5SF";  //ENTER YOUR WIFI SETTINGS
const char *password = "4w%x2yh+bmd2";

//Link to read data from https://script.google.com/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec
//Web/Server address to read/write from 
const char *host = "script.google.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "12 B0 59 D4 F6 FB CD 67 50 13 A4 9E 44 CF 05 3F d7 73 A0 7F";
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  String magstrip = userInput();

    if(magstrip != ""){
    
    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
  
    Serial.println(host);
  
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
        delay(100);
        Serial.print(".");
        r++;
    }
    if(r==30) {
      Serial.println("Connection failed");
    }
    else {
      Serial.println("Connected to web");
    }
    
    String getData, Link;
    //GET Data
    Link = "/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec?id=Att&item1=" + magstrip + "&item2=2&ndplr=1"; //ndplr makes it wo authentication
    Serial.print("requesting URL: ");
    Serial.println(host+Link);
  
    httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +               
                 "Connection: close\r\n\r\n");
  
    Serial.println("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
  
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    String redirect = "null";
    int endOfURL;
    Serial.println("httpsClient.avalable() = " + httpsClient.available());  
    while(httpsClient.available()){   
         
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
      
      if(line[0] == 'T'){
        for(int i=0; i<line.length(); i++){
          Serial.println(line.substring(i, i+6));
          if(line.substring(i, i+6) == "A HREF"){
            i+=8;
            Serial.println("HERE");
            for(int j = i; j < line.length(); j++){
              if(line[j] == '"'){
                endOfURL = j;
                Serial.println("endOfURL = " + String(endOfURL));
                break;
              }
            }
            redirect = line.substring(i, endOfURL);
            Serial.print("i = " + String(i));
            Serial.println(redirect);
            break;
          }
        }
      }
    }
  
    if(redirect != "null"){
      String redirectHost = "script.googleusercontent.com";
      httpsClient.print(String("GET ") + redirect + " HTTP/1.1\r\n" +
                 "Host: " + redirectHost + "\r\n" +               
                 "Connection: close\r\n\r\n");
    }
    
    Serial.println("request sent");
                    
    while (httpsClient.connected()) {
      String line = httpsClient.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
  
    while(httpsClient.available()){   
         
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    
    Serial.println("==========");
    Serial.println("closing connection");
  
  }
    
  delay(2000);  //GET Data at every 2 seconds
}

public String void userInput(){
  //a function which checks for a keyboard input of  magstrip --> outputs the magstrip without 
}
