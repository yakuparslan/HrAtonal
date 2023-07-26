#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <time.h>
#include <NTPClient.h>
#include <Notes.h>

// NTP settings
const char* ntpServer = "pool.ntp.org";
struct tm timeinfo;
int year,month,day,hour,minute,second;


char ssid1[] = "totoo";       
char pass1[] = "totoo119*";

char ssid2[] = "totoo";       
char pass2[] = "totoo119*";


char packetBuffer[255]; 

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const unsigned int localPort = 1234;       

//It will react as play/stop button 
int play  = 0;
int start = 0;

// delay of motor 
int delayMotor = 30;

//Output Pin
const int outputPin = 15;

int counter = 0;
// Default MotorState
int motorState = LOW;  

// Default BPM
int bpm = 120; 

// BPM -> Beat Per Minute -> one beat in miliseconds
long interval = 60000/bpm; 
unsigned long currentMillis = millis();
unsigned long begin = 0;
unsigned long previousMillis = 0; 



void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid1, pass1);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (attempts >= 10) {
      Serial.println("Unable to connect to first Wi-Fi network. Trying the second one...");
      break;
    }
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wi-Fi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    WiFi.begin(ssid2, pass2);
    attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      if (attempts >= 10) {
        Serial.println("Unable to connect to second Wi-Fi network.");
        break;
      }

      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to Wi-Fi!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Unable to connect to any Wi-Fi network.");
    }
  }
}

void printLocalTime(){
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(2000);
    Serial.print(".");
    delay(500);
    Serial.print(".");
    delay(2000);
    Serial.print(".");
    return;
  }
  year = timeinfo.tm_year+1900;
  day = timeinfo.tm_mday;
  month = timeinfo.tm_mon + 1;
  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;
  second = timeinfo.tm_sec;
  Serial.println(String(year)+' '+String(month)+" "+String(day)+' '+String(hour)+':'+String(minute)+':'+String(second));
}

void setup() {
  Serial.begin(115200);
  // Connect to WiFi network
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  WiFi.setHostname(hostname); //define hostname
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  connectToWiFi();
  
  Udp.begin(localPort);
  Serial.print("UDP Started: ");
  Serial.println(localPort);
  Serial.println("Size of the Notes Array: "+String(array_length));
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, motorState);
  delay(3000);
  configTime(7200, 0, ntpServer);

  printLocalTime();
}



void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    String receiver = String(packetBuffer);
    Serial.println(receiver);
    if (receiver == "STOP") {
        play  = 0;
        start = 0; 
        digitalWrite(outputPin, LOW);      
    }
    else if (receiver == "PUSH") {
        currentMillis=millis();
        while(millis()-currentMillis<=delayMotor) {
            digitalWrite(outputPin, HIGH); 
        }
            digitalWrite(outputPin,LOW);
    }
    else if (receiver.substring(0,5) == "DELAY") {
        delayMotor = receiver.substring(6).toInt();
    }
    else if (receiver == "RESET") {
        ESP.restart(); 
    }
    else if (receiver.substring(0,3) == "BPM")  {
        int new_bpm = receiver.substring(4).toInt();     
        if (bpm !=new_bpm&& new_bpm >=1 && new_bpm<=1000) {
              bpm = new_bpm;  
        }
    }
    else if (receiver == "REC" ) {
        play =1;
    }
    else if (receiver == "START" ) {
        start =1;
    }
    else if (strncmp(packetBuffer, "T:", 2) == 0) {
        // Extract the time delay from the packet
       
        int timeDelay = atoi(packetBuffer + 2);
        Serial.println(String(timeDelay));
    }
        receiver = "";
}
  if(play==1){
  interval = 60000/bpm; 
  currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        digitalWrite(outputPin, HIGH);
        delay(delayMotor);
        digitalWrite(outputPin, LOW); 
    }      
  }

  if(start==1){
    if(counter==0){
      begin=millis();
    }
    interval = notes_on[counter]; 
    currentMillis = millis();
    
    if (currentMillis - begin >= interval) {
      digitalWrite(outputPin, HIGH);
      delay(notes_off[counter]-notes_on[counter]);
      digitalWrite(outputPin, LOW); 
      counter++; 
    }
    if(counter==array_length){
      start=0;
      counter=0;
    }   
  } 
    

}

