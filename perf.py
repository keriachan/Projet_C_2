import pandas as pd
import matplotlib.pyplot as plt

def acquerir_stats(nom_fichier):
    stats_list = []
    current_stat = {}
    try:
        with open(nom_fichier, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line:
                    if current_stat:
                        stats_list.append(current_stat)
                        current_stat = {}
                    continue
                parts = line.split()
                if len(parts) >= 2:
                    key = parts[0]
                    value = parts[1] 
                    if key == "cumul_alloc":
                        current_stat[key] = int(value)
                    elif key == "cumul_desalloc":
                        current_stat[key] = int(value)
                    elif key == "max_alloc":
                        current_stat[key] = int(value)
                    elif key == "temps_ecoule":
                        current_stat[key] = float(value)
                    else:
                        current_stat[key] = value
            if current_stat:
                stats_list.append(current_stat)
    except FileNotFoundError:
        print(f"Le fichier {nom_fichier} n'existe pas.")
        return []
    return stats_list

query = ""

while query != "stop":
    query = ""
    while query != "1" and query!="2" and query!="stop":
        print("Que voulez vous faire ? ('stop' pour arrêter)")
        print("1/ Histogrammes mémoire/temps d'exécution par algorithme (les 3 algorithmes doivent avoir leurs fichier de perf nommés stats1, stats2, stats3)")
        print("2/ Graphique de l'évolution de la mémoire allouée par rapport aux opérations d'allocation (pour un algorithme)")
        query = input()
    if query == '1':
        algos = ['Algo 1', 'Algo 2', 'Algo 3']
        stats1 = acquerir_stats('stats1')
        stats2 = acquerir_stats('stats2')
        stats3 = acquerir_stats('stats3')
        if stats1 == [] or stats2 == [] or stats3 == []:
            print("L'un des fichiers n'est pas correctement initialisé")
            break
        print("Pour quelle stat voulez vous générer un histogramme ? ('stop' pour arrêter)")
        print("1/ Max alloué")
        print("2/ Temps d'exécution")
        query = input()
        if query == '1':
            L_maxAlloc = []
            L_maxAlloc.append(stats1[0]['max_alloc'])
            L_maxAlloc.append(stats2[0]['max_alloc'])
            L_maxAlloc.append(stats3[0]['max_alloc'])
            plt.bar(algos, L_maxAlloc)
            plt.title("Histogramme du maximum d'allocations")
            plt.ylabel("Max allocations")
            plt.show()
        elif query == '2':
            L_temps = []
            L_temps.append(stats1[0]['temps_ecoule'])
            L_temps.append(stats2[0]['temps_ecoule'])
            L_temps.append(stats3[0]['temps_ecoule'])
            plt.bar(algos, L_temps)
            plt.title("Histogramme du temps d'exécution (en secondes)")
            plt.ylabel("Temps d'exécution (en sec)")
            plt.show()
        elif query == "stop":
            break
    elif query == '2':
        query = input("Quel algorithme voulez-vous examiner ? (1, 2, 3 ou stop) : ")
        if query == "1":
            df = pd.read_csv("./logs/mem_log_algo1.csv")
            plt.figure(figsize=(12, 6))
            plt.plot(df['operation'], df['memory'], label='Utilisation Mémoire')
            plt.title("Informations mémoire")
            plt.xlabel("Nombre d'opérations")
            plt.ylabel("Mémoire allouée (Octets)")
            plt.legend()
            plt.show()
        elif query == "2":
            df = pd.read_csv("./logs/mem_log_algo2.csv")
            plt.figure(figsize=(12, 6))
            plt.plot(df['operation'], df['memory'], label='Utilisation Mémoire')
            plt.title("Informations mémoire")
            plt.xlabel("Nombre d'opérations")
            plt.ylabel("Mémoire allouée (Octets)")
            plt.legend()
            plt.show()
        elif query == "3":
            df = pd.read_csv("./logs/mem_log_algo3.csv")
            plt.figure(figsize=(12, 6))
            plt.plot(df['operation'], df['memory'], label='Utilisation Mémoire')
            plt.title("Informations mémoire")
            plt.xlabel("Nombre d'opérations")
            plt.ylabel("Mémoire allouée (Octets)")
            plt.legend()
            plt.show()
        elif query == "stop":
            break
    elif query == "stop":
        break
    