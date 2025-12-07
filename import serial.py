import serial
import time
import csv
import os

# -------------------------------
# CONFIGURACIÓN DEL EXPERIMENTO
# -------------------------------

PORT = "COM3"          # Cambia al puerto del RX
BAUD = 115200

DISTANCIA = 400.0        # <--- CAMBIA MANUALMENTE
SPREADING_FACTOR = 12    # <--- CAMBIA MANUALMENTE

INTERVALO = 60          # segundos

CSV_FILE = "resultados_lora.csv"

print("\n=== MEDICIÓN LoRa (60 segundos) ===")
print(f"Distancia = {DISTANCIA} m  |  SF = {SPREADING_FACTOR}")
print("CONECTANDO AL RX...\n")

# ------------------------------------
# ABRIR SERIAL
# ------------------------------------
ser = serial.Serial(PORT, BAUD, timeout=1)

# ------------------------------------
# ABRIR CSV EN MODO ACUMULATIVO
# ------------------------------------
nuevo_archivo = not os.path.exists(CSV_FILE)

with open(CSV_FILE, "a", newline="") as f:
    writer = csv.writer(f)

    if nuevo_archivo:
        writer.writerow(["distancia", "SF", "RSSI_promedio"])

    print("→ Capturando RSSI durante 60 segundos...\n")

    acumulado = 0
    contador = 0
    inicio = time.time()

    # CAPTURA POR 60s
    while time.time() - inicio < INTERVALO:
        try:
            linea = ser.readline().decode().strip()

            if "RSSI" in linea:
                # Formato esperado:
                # RX -> ... | RSSI:-47 | Recibidos:X | Perdidos:Y
                partes = linea.split("|")
                rssi = partes[1].replace("RSSI:", "").strip()

                try:
                    valor = int(rssi)
                    acumulado += valor
                    contador += 1
                except:
                    pass

        except:
            pass

    # CALCULO DEL PROMEDIO
    if contador > 0:
        rssi_promedio = acumulado / contador
    else:
        rssi_promedio = None

    print("=== RESULTADO DE LA MEDICIÓN ===")
    print(f"Distancia: {DISTANCIA} m")
    print(f"Spreading Factor: {SPREADING_FACTOR}")
    print(f"RSSI promedio: {rssi_promedio}")
    print("===============================\n")

    # GUARDAR UNA SOLA FILA
    writer.writerow([DISTANCIA, SPREADING_FACTOR, rssi_promedio])
    f.flush()

    print("Fila guardada en CSV.")
    print("Fin de la medición.\n")
