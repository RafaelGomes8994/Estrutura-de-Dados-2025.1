import random
import string

# --- Configurações ---
NUM_OPERACOES = 50000
# Para garantir duplicatas, usamos um número de nomes únicos menor que o total de operações
NUM_NOMES_UNICOS = 25000
NOME_ARQUIVO_SAIDA = 'arvorebinaria.input.50k.txt'

def gerar_nome_aleatorio():
    """Gera um nome de arquivo aleatório com extensão."""
    # Gera uma string aleatória de 5 a 12 letras minúsculas
    nome_base = ''.join(random.choice(string.ascii_lowercase) for _ in range(random.randint(5, 12)))
    # Escolhe uma extensão aleatória
    extensao = random.choice(['.txt', '.dat', '.log', '.bin', '.bak', '.tmp'])
    return nome_base + extensao

print(f"Gerando {NUM_NOMES_UNICOS} nomes de arquivos únicos...")

# Cria uma lista de nomes de arquivos para garantir que teremos colisões (nomes repetidos)
lista_nomes = []
while len(lista_nomes) < NUM_NOMES_UNICOS:
    novo_nome = gerar_nome_aleatorio()
    if novo_nome not in lista_nomes:
        lista_nomes.append(novo_nome)

print("Nomes gerados. Escrevendo arquivo de saída...")

# Abre o arquivo de saída para escrita
with open(NOME_ARQUIVO_SAIDA, 'w') as f:
    # Escreve o número total de operações na primeira linha
    f.write(f"{NUM_OPERACOES}\n")

    # Gera cada uma das 50.000 linhas de operação
    for _ in range(NUM_OPERACOES):
        nome_arquivo = random.choice(lista_nomes)
        permissao = random.choice(['ro', 'rw'])
        tamanho = random.randint(1, 9999999)
        
        # Escreve a linha formatada no arquivo
        f.write(f"{nome_arquivo} {permissao} {tamanho}\n")

print("-" * 30)
print(f"Arquivo '{NOME_ARQUIVO_SAIDA}' gerado com sucesso!")
print(f"Total de linhas: {NUM_OPERACOES + 1}")
print("-" * 30)