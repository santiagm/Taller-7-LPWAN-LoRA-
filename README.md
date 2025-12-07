# Taller-7-LPWAN-LoRA-

Este repositorio contiene el material utilizado en el Taller 7 de la asignatura Redes de Sensores, cuyo objetivo fue implementar y analizar un enlace LoRa Point-to-Point (P2P) utilizando módulos Heltec WiFi LoRa 32 V3 basados en ESP32-S3.

El experimento incluyó la transmisión inalámbrica de datos simulados de un sensor de temperatura, la captura del RSSI recibido a distintas distancias y valores de Spreading Factor (SF), así como el cálculo del tiempo en aire y las pérdidas de paquetes para cada configuración evaluada.



## Descripción del proyecto

El enlace LoRa P2P se configuró en la banda de 868 MHz utilizando parámetros ajustables como frecuencia, ancho de banda, potencia de transmisión y Spreading Factor (SF). Las pruebas se realizaron a distancias de 20, 120, 200, 350 y 400 metros, manteniendo potencia fija de 10 dBm.

Las principales métricas obtenidas fueron:

- Potencia recibida (RSSI)
- Número de paquetes recibidos y perdidos
- Tiempo en aire para distintos SF
- Intervalos de confianza del 95 % para el ToA

El análisis permitió evaluar la relación entre distancia, SF, robustez del enlace y ocupación del canal.


## Scripts incluidos
### LoRaSender.ino
Programa que envia datos aleatorios de temperatura, enumera los paquetes en orden secuencial.

### LoRaReceiver.ino
Programa que recibe los paquetes de manera secuencial, y mueyra la potencia de recepción.

### captura_rssi.py
Script encargado de leer datos del puerto serial, filtrar líneas con valores RSSI y calcular el promedio para cada distancia y SF. Los resultados se almacenan en un archivo CSV.

### tiempo_aire.py
Script que procesa los tiempos entre paquetes, genera estadísticas de mínimo, máximo y promedio, y construye gráficas con intervalos de confianza del 95 %.

---

## Gráficas generadas

Las siguientes imágenes deben colocarse dentro del directorio `graficas/`:

- `rssi_vs_distancia.png`: Relación distancia vs potencia recibida (RSSI)
- `tiempo_aire_ic95.png`: Tiempo de aire promedio con intervalo de confianza del 95 %
- `paquetes_perdidos.png`: Paquetes perdidos por Spreading Factor
- `oled_display.png`: Captura del funcionamiento de la pantalla OLED en los dispositivos Heltec

Estas gráficas se utilizan en el informe para analizar el impacto del SF y la distancia sobre el desempeño del enlace.

---

## Hardware utilizado

- Heltec WiFi LoRa 32 V3 (ESP32-S3)
- Antena LoRa 868 MHz
- PC con Arduino IDE y Python 3.10
- Librerías Heltec para pantalla OLED y comunicación LoRa

---

## Informe del proyecto

El informe final se encuentra en la carpeta `informe/` bajo el nombre:
Incluye marco teórico, descripción de la práctica, configuración del enlace, algoritmos utilizados, resultados experimentales y conclusiones.

---

## Autores

- Mateo Eduardo Bermeo Pesántez  
- Santiago Andrés Guillén Malla  
- Vicente Paul Jiménez Ávila  

Facultad de Ingeniería – Telecomunicaciones  
Universidad de Cuenca

---



