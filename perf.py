import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import os

def clear_terminal():
    os.system('cls' if os.name == 'nt' else 'clear')

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

def acquerir_res(nom_fichier):
    res_list = []
    current_stat = {}
    dict_mots = {}
    try:
        with open(nom_fichier, "r", encoding="utf-8") as f:
            dict_mots = {}
            for line in f:
                line = line.strip()
                if not line:
                    if current_stat:
                        current_stat['Mots'] = dict_mots
                        res_list.append(current_stat)
                        current_stat = {}
                        dict_mots = {}
                    continue
                parts = line.split()
                if len(parts) >= 2:
                    key = parts[0]
                    value = parts[1] 
                    if key == "Fichier":
                        current_stat[key] = value
                    else:
                        dict_mots[key] = int(value)
            if current_stat:
                current_stat['Mots'] = dict_mots
                res_list.append(current_stat)
    except FileNotFoundError:
        print(f"Le fichier {nom_fichier} n'existe pas.")
        return []
    return res_list

query = ""

while query != "stop":
    query = ""
    while query != "1" and query!="2" and query!="3" and query!="stop":
        clear_terminal()
        print("Que voulez vous faire ? ('stop' pour arrêter)")
        print("1/ Diagrammes mémoire/temps d'exécution par algorithme (les 3 algorithmes doivent avoir leurs fichier de perf nommés stats1, stats2, stats3)")
        print("2/ Graphique de l'évolution de la mémoire allouée par rapport aux opérations d'allocation (pour un algorithme)")
        print("3/ Diagrammes des mots les plus courants")
        query = input()
    if query == '1':
        clear_terminal()
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
            algo_fichier = [stat['Algo'] + stat['Fichier'] for stat in stats1 + stats2 + stats3]
            L_maxAlloc = [stat['max_alloc'] for stat in stats1 + stats2 + stats3]
            couleurs = []
            for stat in stats1 + stats2 + stats3:
                if 'algo1' in stat['Algo']:
                    couleurs.append('blue')
                elif 'algo2' in stat['Algo']:
                    couleurs.append('red')
                elif 'algo3' in stat['Algo']:
                    couleurs.append('green')
            blue_patch = mpatches.Patch(color='blue', label='Algo 1')
            orange_patch = mpatches.Patch(color='red', label='Algo 2')
            green_patch = mpatches.Patch(color='green', label='Algo 3')
            mes_legendes = [blue_patch, orange_patch, green_patch]
            plt.bar(algo_fichier, L_maxAlloc, color=couleurs)
            plt.xticks(fontsize=6)
            plt.legend(handles=mes_legendes)
            plt.title("Histogramme du maximum d'allocations")
            plt.ylabel("Max allocations")
            plt.show()
        elif query == '2':
            algo_fichier = [stat['Algo'] + stat['Fichier'] for stat in stats1 + stats2 + stats3]
            L_temps = [stat['temps_ecoule'] for stat in stats1 + stats2 + stats3]
            couleurs = []
            for stat in stats1 + stats2 + stats3:
                if 'algo1' in stat['Algo']:
                    couleurs.append('blue')
                elif 'algo2' in stat['Algo']:
                    couleurs.append('red')
                elif 'algo3' in stat['Algo']:
                    couleurs.append('green')
            blue_patch = mpatches.Patch(color='blue', label='Algo 1')
            orange_patch = mpatches.Patch(color='red', label='Algo 2')
            green_patch = mpatches.Patch(color='green', label='Algo 3')
            mes_legendes = [blue_patch, orange_patch, green_patch]
            plt.bar(algo_fichier, L_temps, color=couleurs)
            plt.xticks(fontsize=6)
            plt.legend(handles=mes_legendes)
            plt.title("Histogramme du temps d'exécution (en secondes)")
            plt.ylabel("Temps d'exécution (en sec)")
            plt.show()
        elif query == "stop":
            break
    elif query == '2':
        clear_terminal()
        query = input("Quel algorithme voulez-vous examiner ? (1, 2, 3 ou stop pour arreter, le fichier doit etre present dans ./logs/) : ")
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
    elif query == '3':
        clear_terminal()
        query = input("Quel algorithme voulez-vous examiner ? (1, 2, 3 ou stop pour arreter, le fichier doit etre present) : ")
        if query == "1":
            clear_terminal()
            res1 = acquerir_res('res1')
            if res1 == []:
                print("Fichier non initialisé")
            else:
                fichiers = [res['Fichier'] for res in res1]
                d_mots = [res['Mots'] for res in res1]
                for i in range(len(fichiers)):
                    print(f"{i+1}/ Diagramme du fichier {fichiers[i]}")
                query = input()
                while query != 'stop' and ( not query.isdigit() or int(query) < 1 or int(query) > len(fichiers)):
                    query = input()
                if query != 'stop':
                    index = int(query) - 1
                    fichier = fichiers[index]
                    mots = d_mots[index].keys()
                    occurences = d_mots[index].values()
                    plt.bar(mots, occurences)
                    plt.xticks(fontsize=6, rotation = 40)
                    plt.title("Histogramme des mots")
                    plt.ylabel("Ocurrences")
                    plt.show()
        elif query == "2":
            clear_terminal()
            res2 = acquerir_res('res2')
            if res2 == []:
                print("Fichier non initialisé")
            else:
                fichiers = [res['Fichier'] for res in res2]
                d_mots = [res['Mots'] for res in res2]
                for i in range(len(fichiers)):
                    print(f"{i+1}/ Diagramme du fichier {fichiers[i]}")
                query = input()
                while query != 'stop' and ( not query.isdigit() or int(query) < 1 or int(query) > len(fichiers)):
                    query = input()
                if query != 'stop':
                    index = int(query) - 1
                    fichier = fichiers[index]
                    mots = d_mots[index].keys()
                    occurences = d_mots[index].values()
                    plt.bar(mots, occurences)
                    plt.xticks(fontsize=6, rotation = 40)
                    plt.title("Histogramme des mots")
                    plt.ylabel("Ocurrences")
                    plt.show()
        elif query == "3":
            clear_terminal()
            res3 = acquerir_res('res3')
            if res3 == []:
                print("Fichier non initialisé")
            else:
                fichiers = [res['Fichier'] for res in res3]
                d_mots = [res['Mots'] for res in res3]
                for i in range(len(fichiers)):
                    print(f"{i+1}/ Diagramme du fichier {fichiers[i]}")
                query = input()
                while query != 'stop' and ( not query.isdigit() or int(query) < 1 or int(query) > len(fichiers)):
                    query = input()
                if query != 'stop':
                    index = int(query) - 1
                    fichier = fichiers[index]
                    mots = d_mots[index].keys()
                    occurences = d_mots[index].values()
                    plt.bar(mots, occurences)
                    plt.xticks(fontsize=6, rotation = 40)
                    plt.title("Histogramme des mots")
                    plt.ylabel("Ocurrences")
                    plt.show()
        elif query == "stop":
            break
    elif query == "stop":
        break
    