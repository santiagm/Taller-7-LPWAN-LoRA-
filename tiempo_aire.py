import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import t

CSV_FILE = "mediciones_sf.csv"

df = pd.read_csv(CSV_FILE)

# columnas SF
sf_cols = [c for c in df.columns if c.startswith("SF")]

# eje x (los SF)
x = np.arange(len(sf_cols))
labels = [int(c.replace("SF", "")) for c in sf_cols]

medias = []
ic_min = []
ic_max = []

for c in sf_cols:
    datos = df[c].dropna().astype(float)

    n = len(datos)
    media = np.mean(datos)
    s = np.std(datos, ddof=1)

    # intervalo de confianza 95%
    t_val = t.ppf(0.975, n-1)
    error = t_val * s / np.sqrt(n)

    ic_low = media - error
    ic_high = media + error

    medias.append(media)
    ic_min.append(ic_low)
    ic_max.append(ic_high)

# Convertir a arrays
medias = np.array(medias)
ic_min = np.array(ic_min)
ic_max = np.array(ic_max)

# Magnitud del error para gráfico de barras
err_lower = medias - ic_min
err_upper = ic_max - medias

plt.figure(figsize=(8,5))

# ✔️ Gráfica de barras
plt.bar(labels, medias, yerr=[err_lower, err_upper], capsize=6, color="skyblue")

plt.xlabel("Spreading Factor (SF)")
plt.ylabel("Intervalo promedio (ms)")
plt.title("Tiempo de Aire con intervalo de Confianza del 95%")
plt.grid(axis="y", alpha=0.4)

# ✔️ Leyendas de máximo y mínimo
for i, sf in enumerate(labels):
    plt.text(sf, ic_max[i], f"Max: {ic_max[i]:.2f}", ha="center", va="bottom", fontsize=8)
    plt.text(sf, ic_min[i], f"Min: {ic_min[i]:.2f}", ha="center", va="top", fontsize=8)

plt.show()
