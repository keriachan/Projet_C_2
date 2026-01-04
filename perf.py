import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("./logs/mem_log_algo2.csv")

plt.figure(figsize=(12, 6))

plt.plot(df['operation'], df['memory'], label='Utilisation Mémoire')

plt.title("Informations mémoire")
plt.xlabel("Nombre d'opérations")
plt.ylabel("Mémoire allouée (Octets)")

plt.show()