#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>  
#include <avr/wdt.h>
#include <Servo.h>

EthernetServer serveur(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};

byte temperature = 0;
byte humidity = 0;

int pinDHT11 = 2;
SimpleDHT11 dht11;

const int R = 6;
const int G = 5;
const int B = 4;

Servo sun;
Servo cloud;
Servo rain;
int sunHeight = 95;
int cloudHeight = 95;
int rainHeight = 95;

byte temperatureOutdoor = 0;
byte humidityOutdoor = 0;

int pinDHT11Outdoor = 3;
SimpleDHT11 dht11Outdoor;

void setup() {
  lcd.begin(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Initializing...");

  sun.attach(9);
  rain.attach(10);
  cloud.attach(11);
  sun.write(0);
  rain.write(0);
  cloud.write(0);
  
  pinMode(R, OUTPUT);
  digitalWrite(R, LOW);
  pinMode(G, OUTPUT);
  digitalWrite(G, LOW);
  pinMode(B, OUTPUT);
  digitalWrite(B, LOW);
  setColor(HIGH, HIGH, HIGH);
  
  Serial.begin (9600);

  Ethernet.begin (mac);
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());
  lcd.clear();
  lcd.print(Ethernet.localIP());
  serveur.begin();
}


void loop() {
  getTempHum();
  
  EthernetClient client = serveur.available();
  
  if (client) {
    Serial.println("Client en ligne\n");

    if (client.connected()) {
      String httpResponse;
      while (client.available()) {      
        char c=client.read();
        httpResponse += c;
        
        Serial.write(c);
        
        if(httpResponse.indexOf("/reboot") != -1) {
          client.println("SUCCESS");
          client.stop();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Rebooting...");
          delay(2000);
          reboot();
        }else if(httpResponse.indexOf("/red") != -1) {
          client.println("SUCCESS");
          client.stop();
          setColor(HIGH, LOW, LOW);
        }else if(httpResponse.indexOf("/green") != -1) {
          client.println("SUCCESS");
          client.stop();
          setColor(LOW, HIGH, LOW);
        }else if(httpResponse.indexOf("/blue") != -1) {
          client.println("SUCCESS");
          client.stop();
          setColor(LOW, LOW, HIGH);
        }else if(httpResponse.indexOf("/data") != -1){
          //Header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json;charset=utf-8");
          client.println("Server: Arduino");
          client.println("Connnection: close");
          client.println();
    
          //Content
          client.print("{\"outdoor\": {\"temperature\": \"" + (String)temperatureOutdoor + "\", \"humidity\": \"" + (String)humidityOutdoor + "\"}, \"indoor\": {\"temperature\": \"" + (String)temperature + "\", \"humidity\": \"" + (String)humidity + "\"}}");
    
          //Ends
          client.println();
          client.stop();
          Serial.println("Fin de communication avec le client");
        }
        delay(1);
      }

      client.println("Nothing to see here ! Please go to /data");
      client.stop();
    }
  }
}

void getTempHum() {
  lcd.setCursor(0,1);
  lcd.print("Tmp." + (String)temperature + (char)223 + "C " + "Hum." + (int)humidity + "%");
  
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.print("Read DHT11 failed");
    return;
  }

  byte dataOutdoor[40] = {0};
  if (dht11Outdoor.read(pinDHT11Outdoor, &temperatureOutdoor, &humidityOutdoor, dataOutdoor)) {
    Serial.print("Read DHT11 failed");
    return;
  }

  delay(1000);

  //manage
  if(temperatureOutdoor > 18 && humidityOutdoor < 60) {
    if(rain.read() != rainHeight) {
      rain.write(rainHeight); 
    }
    //TODO continue
    cloud.write(0);
    sun.write(sunHeight);
  }else if(temperatureOutdoor < 20 && humidityOutdoor >= 85) {
    sun.write(0);
    cloud.write(0);
    rain.write(rainHeight);
  }else {
    sun.write(0);
    cloud.write(cloudHeight);
    rain.write(0);
  }
}

void setColor(int red, int green, int blue) {
  digitalWrite(R, red);
  digitalWrite(G, green);
  digitalWrite(B, blue);  
}

void reboot() {
  wdt_enable(WDTO_15MS);
  while(1) {}
}

