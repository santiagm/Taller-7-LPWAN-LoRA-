// --------------------------------------------------------------
//  RECEPTOR LoRa - Heltec WiFi LoRa 32(V3)
//  Secuencia + contador RX + pérdida automática
//  + PROMEDIO RSSI CADA 60s (solo Serial)
// --------------------------------------------------------------

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "HT_SSD1306Wire.h"

SSD1306Wire pantalla(0x3c, 500000, SDA_OLED, SCL_OLED,
                     GEOMETRY_128_64, RST_OLED);

#define RF_FREQUENCY          868000000
#define LORA_BANDWIDTH        0
#define LORA_SPREADING_FACTOR 12
#define LORA_CODINGRATE       1
#define LORA_PREAMBLE_LENGTH  8
#define LORA_IQ_INVERSION_ON  false
#define RX_TIMEOUT_VALUE      0

static RadioEvents_t RadioEvents;

char rxpacket[256];
bool receivedFlag = false;

String lastMsg = "";
int16_t lastRSSI = 0;
int8_t lastSNR  = 0;

// ---------- CONTADORES ----------
unsigned long paquetes_recibidos = 0;
long last_seq = -1;
long perdidos_totales = 0;

// ---------- PROMEDIO RSSI (NUEVO) ----------
long acumuladoRSSI = 0;
unsigned long contadorRSSI = 0;

unsigned long t_inicio = 0;
const unsigned long INTERVALO = 60000; // 60 segundos

// ---------- Parámetros ingresados manualmente (NUEVO) ----------
int SF_experimento = 12;         // <--- CAMBIA AQUÍ: 7, 9, 12...
float distancia_metros = 200.0;  // <--- CAMBIA AQUÍ: metros reales


// ---------- ENCENDER OLED ----------
void VextON(void)
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

// ---------- CALLBACK ----------
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';

  lastMsg = String(rxpacket);
  lastRSSI = rssi;
  lastSNR  = snr;

  receivedFlag = true;
  Radio.Sleep();
}

// ---------- SETUP ----------
void setup()
{
  Serial.begin(115200);

  VextON();
  delay(100);

  pantalla.init();
  pantalla.flipScreenVertically();
  pantalla.setFont(ArialMT_Plain_10);
  pantalla.clear();
  pantalla.drawString(0, 0, "Iniciando RX...");
  pantalla.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  RadioEvents.RxDone = OnRxDone;
  Radio.Init(&RadioEvents);

  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetRxConfig(
    MODEM_LORA,
    LORA_BANDWIDTH,
    LORA_SPREADING_FACTOR,
    LORA_CODINGRATE,
    0,
    LORA_PREAMBLE_LENGTH,
    0,
    false,
    0,
    true,
    0, 0,
    LORA_IQ_INVERSION_ON,
    true
  );

  Radio.Rx(0);

  t_inicio = millis();   // <--- INICIO DEL INTERVALO
}

// ---------- LOOP ----------
void loop()
{
  Radio.IrqProcess();

  // ----------- CUANDO LLEGA UN PAQUETE -----------
  if (receivedFlag)
  {
    paquetes_recibidos++;

    // ---------- EXTRAER SECUENCIA ----------
    int c1 = lastMsg.indexOf(':');
    int c2 = lastMsg.indexOf(':', c1 + 1);

    long seq_rx = lastMsg.substring(c1 + 1, c2).toInt();

    // ---------- DETECCIÓN DE PÉRDIDAS ----------
    if (last_seq != -1 && seq_rx != last_seq + 1)
    {
      long perdidos = seq_rx - (last_seq + 1);
      perdidos_totales += perdidos;

      Serial.printf("⚠ Pérdida detectada: faltan %ld paquetes | Total perdidos: %ld\n",
                    perdidos, perdidos_totales);
    }

    last_seq = seq_rx;

    // ---------- ACUMULAR PARA PROMEDIO RSSI (NUEVO) ----------
    acumuladoRSSI += lastRSSI;
    contadorRSSI++;

    // ---------- SERIAL ----------
    Serial.printf("RX -> %s | RSSI:%d | Recibidos:%lu | Perdidos:%ld\n",
                  lastMsg.c_str(), lastRSSI,
                  paquetes_recibidos, perdidos_totales);

    // ---------- OLED (sin cambios) ----------
    pantalla.clear();
    pantalla.setTextAlignment(TEXT_ALIGN_LEFT);
    pantalla.setFont(ArialMT_Plain_10);
    pantalla.drawString(0, 0, "Mensaje:");

    pantalla.setTextAlignment(TEXT_ALIGN_CENTER);
    pantalla.setFont(ArialMT_Plain_10);
    pantalla.drawString(64, 20, lastMsg);

    pantalla.setFont(ArialMT_Plain_10);
    pantalla.drawString(64, 50, "RSSI: " + String(lastRSSI));
    pantalla.display();

    receivedFlag = false;
    Radio.Rx(0);
  }

  // =====================================================
  //      CALCULAR PROMEDIO RSSI CADA 60 SEGUNDOS  (NUEVO)
  // =====================================================
  if (millis() - t_inicio >= INTERVALO)
  {
    if (contadorRSSI > 0)
    {
        float rssi_promedio = (float)acumuladoRSSI / contadorRSSI;

        Serial.println("-------------------------------------------------");
        Serial.printf("RESUMEN DE LA MEDICION (últimos 60s)\n");
        Serial.printf("Distancia: %.1f metros\n", distancia_metros);
        Serial.printf("Spreading Factor (SF): %d\n", SF_experimento);
        Serial.printf("RSSI promedio: %.2f dBm\n", rssi_promedio);
        Serial.printf("Paquetes recibidos: %lu\n", paquetes_recibidos);
        Serial.printf("Paquetes perdidos acumulados: %ld\n", perdidos_totales);
        Serial.println("-------------------------------------------------");

        // ---- Reiniciar acumuladores ----
        acumuladoRSSI = 0;
        contadorRSSI = 0;
    }

    t_inicio = millis(); // Reiniciar el intervalo
  }
}
