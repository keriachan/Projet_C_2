import pandas as pd
import matplotlib.pyplot as plt

query = ""

while query != "stop":
    query = input("Quel algorithme voulez-vous examiner ? (1, 2, 3 ou stop) : ")
    if query == "1":
        df = pd.read_csv("./logs/mem_log_algo1.csv")
    elif query == "2":
        df = pd.read_csv("./logs/mem_log_algo2.csv")
    elif query == "3":
        df = pd.read_csv("./logs/mem_log_algo3.csv")
    elif query == "stop":
        break
    else:
        print("Choix invalide.")
        continue
    plt.figure(figsize=(12, 6))
    plt.plot(df['operation'], df['memory'], label='Utilisation Mémoire')
    plt.title("Informations mémoire")
    plt.xlabel("Nombre d'opérations")
    plt.ylabel("Mémoire allouée (Octets)")
    plt.legend()
    plt.show()