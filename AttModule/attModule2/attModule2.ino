#include <hidboot.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

String magStrip = "";
boolean newMagStripFlag = false;
String newMagStripString = "";
/* Set these to your desired credentials. */
const char *ssid = "Rubin House_2GEXT";  //ENTER YOUR WIFI SETTINGS
const char *password = "bocaboys";

//Link to read data from https://script.google.com/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec
//Web/Server address to read/write from 
const char *host = "script.google.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "12 B0 59 D4 F6 FB CD 67 50 13 A4 9E 44 CF 05 3F d7 73 A0 7F";

class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);

  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  //Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
  //Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
  //Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");
  //Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");

  //Serial.print(" >");
  //PrintHex<uint8_t>(key, 0x80);
  //Serial.print("< ");

  //Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");
  //Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
  //Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");
  //Serial.print((mod.bmRightGUI    == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  //Serial.print("DN ");
  PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;
/*
  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    Serial.println("LeftCtrl changed");
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
    Serial.println("LeftShift changed");
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
    Serial.println("LeftAlt changed");
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
    Serial.println("LeftGUI changed");
  }

  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
    Serial.println("RightCtrl changed");
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {
    Serial.println("RightShift changed");
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
    Serial.println("RightAlt changed");
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
    Serial.println("RightGUI changed");
  }
*/
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  //Serial.print("UP ");
  //PrintKey(mod, key);
}
String newMagStrip(char key)
{

  if((char)key == ';' || ((char)key == '%')){
    newMagStripFlag = true;
    newMagStripString = "";
    newMagStripString += key;
    return "-->";
  }
  else if((char)key == '?'){
    newMagStripFlag = false;
    newMagStripString += key;
    return newMagStripString;
  }
  else if(newMagStripFlag){
    newMagStripString += key;
  }
  else{ return "";}
  return "";
  
}

// %B6010568177851632^1^25010004000060104344           ? %B374327073117140^YOU/A GIFT FOR            ^2303122140170915?;374327073117140=230312214017091500000? %B374327073117140^YOU/A GIFT FOR
//^2303122140170915?;374327073117140=230312214017091500000?
//%B374327073117140^YOU/A GIFT FOR            ^2303122140170915?;374327073117140=230312214017091500000?



void KbdRptParser::OnKeyPressed(uint8_t key)
{

  //Serial.println((char)key);
  //Serial.print("ASCII: ");
/*
  if((char)key != '?'){
    Serial.print((char)key);
  }
  else{
    Serial.println((char)key);
  }
*/
  String magStrip = newMagStrip((char)key);
  
  if(magStrip != "" || magStrip != "-->"){
    Serial.println(magStrip);

}

}

USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

KbdRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
  Serial.println("");
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );
  Serial.println("Moving from USB Host to Wifi");
    
  HidKeyboard.SetReportParser(0, &Prs);

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

void loop()
{
  
  Usb.Task();
      
   if(magStrip != "" && magStrip != "-->"){
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
    Link = "/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec?id=Att&item1=" + magStrip + "&item2=2&ndplr=1"; //ndplr makes it wo authentication
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
  else{

    Serial.println(magStrip);
     
  }
  delay(2000);  //GET Data at every 2 seconds
}
