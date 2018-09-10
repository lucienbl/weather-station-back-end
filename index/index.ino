#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xDF, 0xAB};

EthernetServer serveur(80);

void setup() {
  Serial.begin (9600);
  Ethernet.begin (mac);
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP());
  serveur.begin();
}


void loop() {
  EthernetClient client = serveur.available();
  
  if (client) {
    Serial.println("Client en ligne\n");

    if (client.connected()) {
      while (client.available()) {
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
      client.print("{\"name\": \"Lucien\"}");

      //Ends
      client.println();
      client.stop();
      Serial.println("Fin de communication avec le client");
    }
  }
}
