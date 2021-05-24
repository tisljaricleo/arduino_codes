#include <SoftwareSerial.h> //Koristi serial library
SoftwareSerial pmsSerial(2, 3);//TX RX linija za slanje te linija za primanje

void setup() {

  Serial.begin(115200);//Linija za primanje

  // baud rate senzora 9600
  pmsSerial.begin(9600);//Linija za komunkikaciju s PMS senzorom
}

struct pms5003data {//deklaracija uint16_t koji ima vrijednost od 0 do 65,535
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};//stvaranje strukture podataka

struct pms5003data data;

void loop() {
  if (readPMSdata(&pmsSerial)) {//Ako dolaze nekakvi podatci preko primajuće linije onda


    Serial.println("---------------------------------------");
    Serial.print("Cestica u zraku > 0.3um / 0.1L zraka:");
    Serial.println(data.particles_03um);//Čestica većih od 0.3 um
    Serial.print("Cestica u zraku > 0.5um / 0.1L zraka:");
    Serial.println(data.particles_05um);//Čestica većih od 0.5 um
    Serial.print("Cestica u zraku > 1.0um / 0.1L zraka:");
    Serial.println(data.particles_10um);//Čestica većih od 1 um
    Serial.print("Cestica u zraku > 2.5um / 0.1L zraka:");
    Serial.println(data.particles_25um);//Čestica većih od 2.5 um
    Serial.print("Cestica u zraku > 5.0um / 0.1L zraka:");
    Serial.println(data.particles_50um);//Čestica većih od 5 um
    Serial.print("Cestica u zraku > 10.0 um / 0.1L zraka:");
    Serial.println(data.particles_100um);//Čestica većih od 10 um
    Serial.println("---------------------------------------");
  }
}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  //čitaj byte po bit dok ne dobijemo '0x42' startni byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Sada pročitaj sva 32 bytes
  if (s->available() < 32) {
    return false;
  } uint8_t buffer[32]; uint16_t sum = 0; s->readBytes(buffer, 32);

  // provjerimo sve s kontrolnim zbrojem
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }



  // Podatci dolaze u endian'd protokolu, ovaj dio koda omogućava podržanost na svim platformama
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // spremimo sve u strukturu
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // kada sve uspije vraćamo rezultat
  return true;
}
