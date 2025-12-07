// --------------------------------------------------------------
//  TRANSMISOR LoRa - Heltec WiFi LoRa 32(V3)
//  Con secuencia + contador de paquetes enviados
//  (OLED sin cambios)
// --------------------------------------------------------------

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "HT_SSD1306Wire.h"

// Pantalla
SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED,
                     GEOMETRY_128_64, RST_OLED);

// Configuración LoRa
#define RF_FREQUENCY          900000000
#define TX_OUTPUT_POWER       10
#define LORA_BANDWIDTH        0
#define LORA_SPREADING_FACTOR 10
#define LORA_CODINGRATE       1
#define LORA_PREAMBLE_LENGTH  8
#define LORA_IQ_INVERSION_ON  false
#define RX_TIMEOUT_VALUE      1000
#define BUFFER_SIZE           30

char txpacket[BUFFER_SIZE];
bool lora_idle = true;

static RadioEvents_t RadioEvents;

unsigned long seq = 0;              // <--- NÚMERO DE SECUENCIA
unsigned long paquetes_enviados = 0; // <--- CONTADOR TX

void OnTxDone(void)
{
  Serial.println("TX done");
  lora_idle = true;
}

void OnTxTimeout(void)
{
  Serial.println("TX timeout");
  lora_idle = true;
}

void VextON(void)
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void setup()
{
  Serial.begin(115200);

  VextON();
  delay(100);

  // OLED sin cambios
  pantalla.init();
  pantalla.flipScreenVertically();
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.clear();
  pantalla.drawString(0, 0, "Iniciando TX LoRa...");
  pantalla.display();

  // LoRa
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetTxConfig(
    MODEM_LORA,
    TX_OUTPUT_POWER,
    0,
    LORA_BANDWIDTH,
    LORA_SPREADING_FACTOR,
    LORA_CODINGRATE,
    LORA_PREAMBLE_LENGTH,
    false,
    true,
    0, 0,
    LORA_IQ_INVERSION_ON,
    3000
  );
}

void loop()
{
  if (lora_idle)
  {
    delay(1000);

    double temperatura = random(0, 100);

    // ---------- SECUENCIA ----------
    sprintf(txpacket, "seq:%lu:tp:%0.2f", seq, temperatura);
    seq++;

    // ---------- OLED (sin cambios) ----------
    pantalla.clear();
    pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
    pantalla.setFont(ArialMT_Plain_10);
    pantalla.drawString(0, 0, "Enviando LoRa...");

    pantalla.setTextAlignment(TEXT_ALIGN_CENTER);
    pantalla.setFont(ArialMT_Plain_24);
    pantalla.drawString(64, 20, String(temperatura, 1) + " C");

    pantalla.setFont(ArialMT_Plain_10);
    pantalla.drawString(64, 50, String(txpacket));
    pantalla.display();

    // ---------- ENVÍO ----------
    Radio.Send((uint8_t *)txpacket, strlen(txpacket));
    paquetes_enviados++;

    // ---------- SERIAL ----------
    Serial.printf("TX -> %s | Enviados: %lu\n", txpacket, paquetes_enviados);

    lora_idle = false;
  }

  Radio.IrqProcess();
}
