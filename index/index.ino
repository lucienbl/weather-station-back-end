#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>  

EthernetServer serveur(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};

byte temperature = 0;
byte humidity = 0;

int pinDHT11 = 2;
SimpleDHT11 dht11;

void setup() {
  lcd.begin(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Initializing...");
    
  Serial.begin (9600);

  Ethernet.begin (mac);
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());
  lcd.clear();
  lcd.print("IP: ");
  lcd.setCursor(4, 0);
  lcd.print(Ethernet.localIP());
  serveur.begin();
}


void loop() {
  getTempHum();
  
  EthernetClient client = serveur.available();
  
  if (client) {
    Serial.println("Client en ligne\n");

    if (client.connected()) {
      while (client.available()) {
        lcd.setCursor(0, 1);
        lcd.print("Client loading...");
      
        char c=client.read();
        Serial.write(c);
        delay(1);
      }

      //Header
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json;charset=utf-8");
      client.println("Server: Arduino");
      client.println("Connnection: close");
      client.println();

      //Content
      client.print("{\"temperature\": \"" + (String)temperature + "\", \"humidity\": \"" + (String)humidity + "\"}");

      //Ends
      client.println();
      client.stop();
      Serial.println("Fin de communication avec le client");
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

  delay(1000);
}
