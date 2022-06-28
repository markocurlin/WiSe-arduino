#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>

#define DHTPIN 7
#define DHTTYPE DHT22

class SENSORS {
public:
  SENSORS();
  ~SENSORS();
  void DHT_init();
  void BH1750_init();
  void readTempHum();
  void readLight();
  float getTemperature();
  float getHumidityAir();
  float getHumiditySoil();
  float getLux();
private:
  float temperature;
  float humidityair;
  float humiditysoil;
  uint32_t lux;
};

DHT dht(DHTPIN, DHTTYPE);

BH1750 lightMeter(0x23);

SENSORS::SENSORS() {
    this->temperature = 0;
    this->humidityair = 0;
    this->humiditysoil = 0;
    this->lux = 0;
}
SENSORS::~SENSORS() {}

void SENSORS::DHT_init() {
    Serial.println(F("DHT test!"));
    dht.begin();
}

void SENSORS::BH1750_init() {
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    Serial.println(F("BH1750 test!"));
}

void SENSORS::readTempHum() {
  float humidityair = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidityair) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  } else {
    this->humidityair = humidityair;
    this->temperature = temperature;

    Serial.println(F("Humidity: "));
    Serial.println(humidityair);     
    Serial.println(F("Temperature: "));
    Serial.println(temperature);
  }
}

void SENSORS::readLight() {
    uint32_t lux = lightMeter.readLightLevel();

    this->lux = lux;
    Serial.println(F("Light: "));
    Serial.println(lux);
    Serial.println(F(" lx"));
}

float SENSORS::getTemperature() {
    return this->temperature;
}

float SENSORS::getHumidityAir() {
    return this->humidityair;
}

float SENSORS::getHumiditySoil() {
    return this->humiditysoil;
}

float SENSORS::getLux() {
    return this->lux;
}

SENSORS sensor;

#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
//# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

static const u1_t PROGMEM DEVEUI[8]={0x54, 0x1D, 0x05, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

static const u1_t PROGMEM APPKEY[16] = {0xFE, 0xFD, 0xEF, 0x93, 0x4F, 0xB7, 0x3E, 0xA0, 0x87, 0xF8, 0xAA, 0xE3, 0xE3, 0xFE, 0x5B, 0x44};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[] = {10, 20, 32, 40};

static osjob_t sendjob;

void do_send(osjob_t* j){
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        mydata[0] = sensor.getTemperature();
        mydata[1] = sensor.getHumidityAir();
        mydata[2] = sensor.getLux();
        mydata[3] = 41;
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        Serial.println(F("Packet queued"));
    }
}

const unsigned TX_INTERVAL = 60;

const lmic_pinmap lmic_pins = {
    .nss = 6,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 5,
    .dio = {2, 3, 4},
};

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
            LMIC_setLinkCheckMode(0);
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println(F("Starting"));

    sensor.DHT_init();
    sensor.BH1750_init();

    #ifdef VCC_ENABLE
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    os_init();
    
    LMIC_reset();

    do_send(&sendjob);
    
}

void loop() {
    sensor.readTempHum();
    sensor.readLight();

    os_runloop();
}