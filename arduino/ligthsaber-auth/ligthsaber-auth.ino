/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <PubSubClient.h>

char hexChars[] = "0123456789ABCDEF";
#define HEX_MSB(v) hexChars[(v & 0xf0) >> 4]
#define HEX_LSB(v) hexChars[v & 0x0f]

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x07
};

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          7         // Configurable, see typical pin layout above - set to 10 to avoid conflict with W5100

#define PIN_TWO      2 // button 1
#define PIN_THREE    3 // button 2

IPAddress server(10, 1, 1, 1);

void callback(char* topic, byte* payload, unsigned int length) {
  // nuthing
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

boolean reconnect() {
  Serial.println(F("Reconnecting"));
  if (client.connect("arduinoClient")) {
    // Once connected, publish an announcement...
    client.publish("rfid","CLOSEDOOR");
    // ... and resubscribe
    client.subscribe("rfid");
  }
  return client.connected();
}

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

long lastReconnectAttempt = 0;
byte nuidPICC[4];

int reading_one = 0;
int state_one = HIGH;
int prev_one = LOW;
long debounce = 20;
long time_one = 0;

int reading_two = 0;
int state_two = HIGH;
int prev_two = LOW;
long time_two = 0;

void setup() {

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  delay(1);

  pinMode(PIN_TWO, INPUT);  
  pinMode(PIN_THREE, INPUT);
  
  client.setServer(server, 1883);
  client.setCallback(callback);
  
	Serial.begin(9600);		// Initialize serial communications with the PC
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

	// Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  printIPAddress();

  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.print(F("Success!"));
      client.publish("rfid","CLOSEDOOR");
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  lastReconnectAttempt = 0;
}

char cstr[32];

void loop() {

  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {

    // check button one
    reading_one = digitalRead(PIN_TWO);  
    if(reading_one == HIGH && prev_one == LOW && (millis() - time_one) > debounce) {
      Serial.println(F("Button 1 pushed"));        
      client.publish("rfid","CLOSEDOOR");
      time_one = millis();
    }
    prev_one = reading_one;

    // check button two
    /*
    reading_two = digitalRead(PIN_THREE);  
    if(reading_two == HIGH && prev_two == LOW && (millis() - time_two) > debounce) {
      Serial.println(F("Button 2 pushed"));        
      client.publish("rfid","CLOSEDOOR");
      time_two = millis();
    }
    prev_two = reading_two;
    */
     switch (Ethernet.maintain())
    {
      case 1:
        //renewed fail
        Serial.println("Error: renewed fail");
        break;
  
      case 2:
        //renewed success
        Serial.println("Renewed success");
  
        //print your local IP address:
        printIPAddress();
        break;
  
      case 3:
        //rebind fail
        Serial.println("Error: rebind fail");
        break;
  
      case 4:
        //rebind success
        Serial.println("Rebind success");
  
        //print your local IP address:
        printIPAddress();
        break;
  
      default:
        //nothing happened
        break;
  
    }
    client.loop();
    
  	// Look for new cards
  	if ( ! mfrc522.PICC_IsNewCardPresent()) {
  		return;
  	}
  
  	// Select one of the cards
  	if ( ! mfrc522.PICC_ReadCardSerial()) {
  		return;
  	}

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_UL) {
      Serial.println(F("Your tag is not of type MIFARE Ultralight."));
      return;
    }

    unsigned long hex_num;
    hex_num =  mfrc522.uid.uidByte[0] << 24;
    hex_num += mfrc522.uid.uidByte[1] << 16;
    hex_num += mfrc522.uid.uidByte[2] <<  8;
    hex_num += mfrc522.uid.uidByte[3];

    // Serial.println(hex_num);
    char charTopic[] = "XXXXXXXXXXXXXXXX";
    int i = 0;
    for (i = 0; i < 8; i++) {
      charTopic[0+i*2] = HEX_MSB(mfrc522.uid.uidByte[i]);
      charTopic[1+i*2] = HEX_LSB(mfrc522.uid.uidByte[i]);
    }

    //Serial.println(charTopic);

    // publish the read tag to MQTT
    client.publish("rfid", charTopic);

    // Halt PICC
    mfrc522.PICC_HaltA();
    
  }
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void printIPAddress()
{
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  Serial.println();
}
