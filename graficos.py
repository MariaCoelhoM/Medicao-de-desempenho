import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("resultados.csv")

# 📊 1. Tempo de execução
for n in df['Dimensao'].unique():
    sub = df[df['Dimensao'] == n]
    plt.plot(sub['Threads'], sub['Tempo_Par'], marker='o', label=f'n={n}')

plt.xlabel("Número de Threads")
plt.ylabel("Tempo (s)")
plt.title("Tempo de Execução")
plt.legend()
plt.grid()
plt.savefig("tempo_execucao.png")
plt.clf()


# 📊 2. Speedup
for n in df['Dimensao'].unique():
    sub = df[df['Dimensao'] == n]
    plt.plot(sub['Threads'], sub['Speedup'], marker='o', label=f'n={n}')

plt.xlabel("Número de Threads")
plt.ylabel("Speedup")
plt.title("Speedup")
plt.legend()
plt.grid()
plt.savefig("speedup.png")
plt.clf()


# 📊 3. Eficiência
for n in df['Dimensao'].unique():
    sub = df[df['Dimensao'] == n]
    plt.plot(sub['Threads'], sub['Eficiencia'], marker='o', label=f'n={n}')

plt.xlabel("Número de Threads")
plt.ylabel("Eficiência")
plt.title("Eficiência")
plt.legend()
plt.grid()
plt.savefig("eficiencia.png")  # ✅ estava faltando
plt.clf()

print("Gráficos gerados: tempo_execucao.png, speedup.png, eficiencia.png")