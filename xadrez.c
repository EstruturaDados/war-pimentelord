#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// --- Constantes ---
#define MAX_NOME 50
#define NUM_TERRITORIOS 5
#define DADOS_ATAQUE 3
#define DADOS_DEFESA 2

// --- Estruturas e Tipos ---

/**
 * @brief Representa um território no jogo.
 */
typedef struct {
    char nome[MAX_NOME];
    char cor_exercito[MAX_NOME];
    int num_tropas;
} Territorio;

/**
 * @brief Tipos de Missões possíveis.
 */
typedef enum {
    MISSAO_DESTRUIR_VERDE,
    MISSAO_CONQUISTAR_3, // Conquistar 3 territórios além dos iniciais (Total de 5)
    NUM_MISSOES
} TipoMissao;

// --- Protótipos das Funções ---

// Funções de Utilitários
static void limpar_buffer_entrada();
void inicializar_aleatoriedade();
Territorio *inicializar_mapa(size_t num_territorios);
void configurar_territorios_iniciais(Territorio *mapa, size_t total);
void exibir_mapa(const Territorio *mapa, size_t total);

// Funções de Batalha
static int rolar_dado();
static bool atacante_vence(int dado_ataque, int dado_defesa);
void simular_ataque(Territorio *atacante, Territorio *defensor);

// Funções de Missão e Vitória
TipoMissao sortear_missao();
void exibir_missao(TipoMissao missao);
bool verificar_vitoria(const Territorio *mapa, size_t total, TipoMissao missao);
static bool missao_destruir_exercito(const Territorio *mapa, size_t total, const char *cor_alvo);
static bool missao_conquistar_territorios(const Territorio *mapa, size_t total, const char *cor_jogador, int alvo);


// --- Implementação das Funções ---

/**
 * @brief Limpa o buffer de entrada após o uso do scanf.
 */
static void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void inicializar_aleatoriedade() {
    srand(time(NULL));
}

/**
 * @brief Aloca dinamicamente o mapa com calloc e o inicializa com zero.
 */
Territorio *inicializar_mapa(size_t num_territorios) {
    Territorio *mapa = (Territorio *)calloc(num_territorios, sizeof(Territorio));
    if (mapa == NULL) {
        perror("Erro ao alocar memória para o mapa");
    }
    return mapa;
}

/**
 * @brief Configura nomes e tropas iniciais dos territórios de forma automática.
 */
void configurar_territorios_iniciais(Territorio *mapa, size_t total) {
    // Configuração inicial padrão para garantir que o exército Verde exista para a missão
    const char *nomes[] = {"Alaska", "Groenlandia", "Brasil", "Africa do Sul", "Siberia"};
    const char *cores[] = {"Azul", "Vermelho", "Azul", "Vermelho", "Verde"};
    int tropas[] = {3, 2, 4, 3, 2};

    for (size_t i = 0; i < total; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        strcpy(mapa[i].cor_exercito, cores[i]);
        mapa[i].num_tropas = tropas[i];
    }
    printf("✅ Territórios iniciais configurados automaticamente.\n");
}

/**
 * @brief Exibe o estado atual de todos os territórios.
 */
void exibir_mapa(const Territorio *mapa, size_t total) {
    printf("\n============================================\n");
    printf("              🗺️ ESTADO ATUAL DO MAPA 🗺️\n");
    printf("============================================\n");
    printf("| # | Nome                  | Exército      | Tropas |\n");
    printf("|---|-----------------------|---------------|--------|\n");
    for (size_t i = 0; i < total; i++) {
        // Uso de const char * garante que o mapa não será alterado (const correctness)
        printf("| %zu | %-21s | %-13s | %-6d |\n", 
               i + 1, 
               mapa[i].nome, 
               mapa[i].cor_exercito, 
               mapa[i].num_tropas);
    }
    printf("============================================\n");
}

// --- Funções de Batalha ---

/**
 * @brief Retorna um valor aleatório de um dado (1 a 6).
 */
static int rolar_dado() {
    return (rand() % 6) + 1;
}

/**
 * @brief Simula um único confronto de dados. Empates favorecem o atacante.
 */
static bool atacante_vence(int dado_ataque, int dado_defesa) {
    return dado_ataque >= dado_defesa;
}

/**
 * @brief Simula uma batalha entre dois territórios, atualizando as tropas.
 * @param atacante Ponteiro para o território atacante.
 * @param defensor Ponteiro para o território defensor.
 */
void simular_ataque(Territorio *atacante, Territorio *defensor) {
    // 1. Verificar pré-condições
    if (atacante->num_tropas < 2) {
        printf("\n❌ %s (Atacante) precisa de pelo menos 2 tropas para atacar.\n", atacante->nome);
        return;
    }
    if (defensor->num_tropas == 0) {
        printf("\n⚠️ %s já está sem tropas. Sem necessidade de ataque.\n", defensor->nome);
        return;
    }
    
    // 2. Definir o número de dados
    int num_dados_ataque = (atacante->num_tropas - 1 < DADOS_ATAQUE) ? atacante->num_tropas - 1 : DADOS_ATAQUE;
    int num_dados_defesa = (defensor->num_tropas < DADOS_DEFESA) ? defensor->num_tropas : DADOS_DEFESA;

    printf("\n⚔️ Batalha: %s (%s, %d tropas) vs %s (%s, %d tropas) 🛡️\n", 
        atacante->nome, atacante->cor_exercito, atacante->num_tropas, 
        defensor->nome, defensor->cor_exercito, defensor->num_tropas);
    printf("-> Atacante rola %d dado(s). Defensor rola %d dado(s).\n", num_dados_ataque, num_dados_defesa);

    // 3. Rolar os dados
    int dados_ataque[DADOS_ATAQUE];
    int dados_defesa[DADOS_DEFESA];

    for (int i = 0; i < num_dados_ataque; i++) dados_ataque[i] = rolar_dado();
    for (int i = 0; i < num_dados_defesa; i++) dados_defesa[i] = rolar_dado();

    printf("Dados do Atacante: ");
    for (int i = 0; i < num_dados_ataque; i++) printf("[%d] ", dados_ataque[i]);
    printf("\nDados do Defensor: ");
    for (int i = 0; i < num_dados_defesa; i++) printf("[%d] ", dados_defesa[i]);
    printf("\n");

    // 4. Comparar e aplicar perdas
    int perdas_atacante = 0;
    int perdas_defensor = 0;
    int num_confrontos = (num_dados_ataque < num_dados_defesa) ? num_dados_ataque : num_dados_defesa;

    for (int i = 0; i < num_confrontos; i++) {
        if (atacante_vence(dados_ataque[i], dados_defesa[i])) { 
            perdas_defensor++;
        } else {
            perdas_atacante++;
        }
    }

    atacante->num_tropas -= perdas_atacante;
    defensor->num_tropas -= perdas_defensor;
    
    if (atacante->num_tropas < 1) atacante->num_tropas = 1;
    if (defensor->num_tropas < 0) defensor->num_tropas = 0;

    printf("\nResultados do Confronto:\n");
    printf("  Perdas do Atacante (%s): %d tropa(s)\n", atacante->nome, perdas_atacante);
    printf("  Perdas do Defensor (%s): %d tropa(s)\n", defensor->nome, perdas_defensor);

    // 5. Checar conquista
    if (defensor->num_tropas == 0) {
        printf("\n🎉 CONQUISTA! %s perdeu todas as tropas!\n", defensor->nome);
        printf("-> %s é agora o novo dono de %s.\n", atacante->cor_exercito, defensor->nome);

        // Conquista: Mudar cor e mover pelo menos 1 tropa
        strcpy(defensor->cor_exercito, atacante->cor_exercito);
        
        defensor->num_tropas = 1; 
        atacante->num_tropas--; 
        
        printf("-> Estado Final: %s (%d tropas) | %s (%d tropas)\n", 
               atacante->nome, atacante->num_tropas, 
               defensor->nome, defensor->num_tropas);
    } else {
        printf("A defesa de %s resistiu. Batalha encerrada.\n", defensor->nome);
        printf("-> Estado Final: %s (%d tropas) | %s (%d tropas)\n", 
               atacante->nome, atacante->num_tropas, 
               defensor->nome, defensor->num_tropas);
    }
}

// --- Funções de Missão e Vitória ---

TipoMissao sortear_missao() {
    return (TipoMissao)(rand() % NUM_MISSOES);
}

void exibir_missao(TipoMissao missao) {
    printf("\n📜 Sua Missão Secreta:\n");
    switch (missao) {
        case MISSAO_DESTRUIR_VERDE:
            printf("  ➡️ **DESTRUIR O EXÉRCITO VERDE** e conquistar todos os seus territórios.\n");
            break;
        case MISSAO_CONQUISTAR_3:
            printf("  ➡️ **CONQUISTAR 3 TERRITÓRIOS** (totalizando 5, ou seja, o mapa inteiro).\n");
            break;
        default:
            printf("  ➡️ Missão Indefinida.\n");
            break;
    }
    printf("-------------------------------------------\n");
}

/**
 * @brief Verifica se a missão de eliminar todas as tropas de uma cor foi cumprida.
 */
static bool missao_destruir_exercito(const Territorio *mapa, size_t total, const char *cor_alvo) {
    for (size_t i = 0; i < total; i++) {
        // Se a cor alvo for encontrada, a missão falhou.
        if (strcmp(mapa[i].cor_exercito, cor_alvo) == 0) {
            return false;
        }
    }
    return true; // Todos os territórios do alvo foram conquistados
}

/**
 * @brief Verifica se a missão de conquistar um número total de territórios foi cumprida.
 */
static bool missao_conquistar_territorios(const Territorio *mapa, size_t total, const char *cor_jogador, int alvo) {
    int territorios_do_jogador = 0;
    
    // Contar quantos territórios pertencem ao jogador
    for (size_t i = 0; i < total; i++) {
        if (strcmp(mapa[i].cor_exercito, cor_jogador) == 0) {
            territorios_do_jogador++;
        }
    }

    return territorios_do_jogador >= alvo;
}

/**
 * @brief Verifica qual missão foi sorteada e se a condição de vitória foi atingida.
 */
bool verificar_vitoria(const Territorio *mapa, size_t total, TipoMissao missao) {
    // O jogador principal é o exército Azul, que começa com Alaska e Brasil.
    const char *cor_jogador = "Azul";

    switch (missao) {
        case MISSAO_DESTRUIR_VERDE:
            return missao_destruir_exercito(mapa, total, "Verde");
            
        case MISSAO_CONQUISTAR_3:
            // O mapa tem 5 territórios no total. Se a missão for conquistar 3 a mais, o alvo é 5.
            return missao_conquistar_territorios(mapa, total, cor_jogador, 5); 
            
        default:
            return false;
    }
}


// --- Função Principal (Lógica do Jogo) ---

int main() {
    inicializar_aleatoriedade();

    // 1. Inicialização
    Territorio *mapa = inicializar_mapa(NUM_TERRITORIOS);
    if (mapa == NULL) {
        return 1;
    }

    configurar_territorios_iniciais(mapa, NUM_TERRITORIOS);
    TipoMissao missao_secreta = sortear_missao();
    
    int opcao;
    int atacante_idx, defensor_idx;

    printf("\n--- 🧠 BEM-VINDO AO WAR ESTRUTURADO (NÍVEL MESTRE) 🏆 ---\n");
    exibir_missao(missao_secreta);

    // 2. Loop Principal
    do {
        exibir_mapa(mapa, NUM_TERRITORIOS);

        printf("\n==================== MENU ====================\n");
        printf("1 - Atacar\n");
        printf("2 - Verificar Missão\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer_entrada();
            opcao = 0; // Força a saída em caso de erro de leitura grave
        } else {
            limpar_buffer_entrada();
        }

        switch (opcao) {
            case 1: // Atacar
                printf("\n--- ⚔️ INICIAR ATAQUE ---\n");
                printf("Território Atacante (1 a %d): ", NUM_TERRITORIOS);
                if (scanf("%d", &atacante_idx) != 1) { limpar_buffer_entrada(); break; }
                
                printf("Território Defensor (1 a %d): ", NUM_TERRITORIOS);
                if (scanf("%d", &defensor_idx) != 1) { limpar_buffer_entrada(); break; }
                limpar_buffer_entrada();

                if (atacante_idx >= 1 && atacante_idx <= NUM_TERRITORIOS &&
                    defensor_idx >= 1 && defensor_idx <= NUM_TERRITORIOS &&
                    atacante_idx != defensor_idx) 
                {
                    // Índices de array são (escolha - 1)
                    simular_ataque(&mapa[atacante_idx - 1], &mapa[defensor_idx - 1]);
                } else {
                    printf("⚠️ Seleção inválida. Tente novamente.\n");
                }
                break;

            case 2: // Verificar Missão
                printf("\n--- VERIFICANDO CONDIÇÃO DE VITÓRIA ---\n");
                if (verificar_vitoria(mapa, NUM_TERRITORIOS, missao_secreta)) {
                    printf("\n\n👑 Parabéns! VOCÊ CUMPRIU SUA MISSÃO SECRETA! VITÓRIA! 🏆\n");
                    opcao = 0; // Encerra o jogo
                } else {
                    printf("❌ Missão ainda não foi cumprida. Continue atacando!\n");
                    exibir_missao(missao_secreta);
                }
                break;

            case 0: // Sair
                printf("\nEncerrando o jogo. Obrigado por jogar!\n");
                break;

            default:
                printf("\nOpção inválida. Digite 1, 2 ou 0.\n");
                break;
        }

    } while (opcao != 0);

    // 3. Limpeza
    free(mapa);
    mapa = NULL; 

    return 0;
}