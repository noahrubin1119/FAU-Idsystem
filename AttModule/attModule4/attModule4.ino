/*
 * 
 * Resources: 
 * https://www.hackster.io/139994/plug-any-usb-device-on-an-esp8266-e0ca8a --> how to hook up the USB Hub to the ESP8266 and other boards
 * 
 * 
 * 
 * 
 */



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
String getData, Link;


//Link to read data from https://script.google.com/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec
//Web/Server address to read/write from 
const char *host = "script.google.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "A2 77 8D E8 5B D9 03 DB F9 6F 48 F5 81 C3 7A 8E 7C 7F 1B 54";

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
    Serial.println(key);
    Serial.println("newMagStripFlag: " + String(newMagStripFlag));
    return "";
  }
  else if((char)key == '?'){
    newMagStripFlag = false;
    newMagStripString += key;
    Serial.println(key);
    Serial.println("Returning: " + newMagStripString);
    return newMagStripString;
  }
  else if(newMagStripFlag){
    Serial.println("Adding to the magStrip");
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
  
  if(magStrip != ""){
    Link = "/macros/s/AKfycbxFjSuWk3HR-m_b2GOBr7o5M0UlgzJGnUwOWrhDHkGYD6Wr0T-k/exec?id=Att&item1=" + magStrip + "&item2=2&ndplr=1"; //ndplr makes it wo authentication
    easyHttpsRequest(host, Link); 
  }

}

void easyHttpsRequest(String requestHost, String requestLink){ //accesses a https - reccurs if there is a 
    Serial.println("Inside the easyHttpsRequest: " + requestHost + requestLink);
    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
  
    Serial.println(requestHost);
  
    Serial.printf("Using fingerprint '%s'\n", fingerprint);
    httpsClient.setFingerprint(fingerprint);
    httpsClient.setTimeout(15000); // 15 Seconds
    delay(1000);
    
    Serial.print("HTTPS Connecting");
    int r=0; //retry counter
    while((!httpsClient.connect(requestHost, httpsPort)) && (r < 30)){
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
    
    
    //GET Data
    Serial.print("requesting URL: ");
    Serial.println(requestHost+requestLink);
  
    httpsClient.print(String("GET ") + requestLink + " HTTP/1.1\r\n" +
                 "Host: " + requestHost + "\r\n" +               
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
    while(httpsClient.connected()){   
   // while(httpsClient.readString() != "</body>"){    
      
//      line = httpsClient.readStringUntil('>here</A>');  //Read Line by Line
//      Serial.println(httpsClient.readString());
//      Serial.println("line: " + line); //Print response

           line = httpsClient.readString();
           Serial.println(line);
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
              Serial.println("redirect: " + redirect);
            }
          }
        }
    
  
    if(redirect != "null"){
      String redirectHost = "";
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
         
      line = httpsClient.readString();  //Read Line by Line
      Serial.println(line); //Print response
    }
    
    Serial.println("==========");
    Serial.println("closing connection");
  
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
  delay(5);  //GET Data at every 150 milliseconds
}
