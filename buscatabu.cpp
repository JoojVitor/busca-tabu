#include <iostream>
#include <vector>
#include <random>
#include <ctime>
using namespace std;

const int MAX_ITERATIONS = 1000;
const int TAMANHO_TABU = 5;

// Estrutura para representar uma solução
struct Solucao {
    vector<int> areasSelecionadas;
    int valorObjetivo;
};

// Função para gerar uma solução inicial aleatória
Solucao gerarSolucaoInicialAleatoria(int numAreas) {
    Solucao solucaoInicial;
    solucaoInicial.areasSelecionadas.resize(numAreas);

    // Inicializa o gerador de números aleatórios com a hora atual
    srand(time(0));

    // Gera cada bit aleatoriamente
    for (int i = 0; i < numAreas; i++) {
        int bit = rand() % 2;  // Gera um número aleatório entre 0 e 1
        solucaoInicial.areasSelecionadas[i] = bit;
        cout << bit << " ";  // Adiciona o bit à string
    }

    cout << endl;
    solucaoInicial.valorObjetivo = 0;
    return solucaoInicial;
}

// Função para avaliar a função objetivo
int avaliarObjetivo(const vector<int>& areasSelecionadas, const vector<int>& retornos) {
    int valorObjetivo = 0;
    for (int i = 0; i < areasSelecionadas.size(); i++) {
        valorObjetivo += areasSelecionadas[i] * retornos[i];
    }
    return valorObjetivo;
}

// Função para verificar se uma solução está na lista tabu
bool estaNaListaTabu(const Solucao& solucao, const vector<Solucao>& listaTabu) {
    for (const Solucao& solucaoTabu : listaTabu) {
        if (solucao.areasSelecionadas == solucaoTabu.areasSelecionadas) {
            return true;
        }
    }
    return false;
}

// Função para atualizar a lista tabu
void atualizarListaTabu(vector<Solucao>& listaTabu, const Solucao& solucao) {
    if (listaTabu.size() >= TAMANHO_TABU) {
        listaTabu.erase(listaTabu.begin());
    }
    listaTabu.push_back(solucao);
}

// Algoritmo de busca tabu
Solucao buscaTabu(int numAreas, const vector<int>& investimentos, const vector<int>& retornos, int orcamento) {
    srand(time(0));
    vector<Solucao> listaTabu;
    Solucao melhorSolucao;
    int iteracao = 0;

    // Gerar solução inicial aleatória
    Solucao solucaoAtual = gerarSolucaoInicialAleatoria(numAreas);
    solucaoAtual.valorObjetivo = avaliarObjetivo(solucaoAtual.areasSelecionadas, retornos);
    melhorSolucao = solucaoAtual;

    while (iteracao < MAX_ITERATIONS) {
        // Gerar lista de movimentos vizinhos
        vector<Solucao> vizinhos;
        for (int i = 0; i < numAreas; i++) {
            Solucao vizinho = solucaoAtual;
            vizinho.areasSelecionadas[i] = 1 - vizinho.areasSelecionadas[i]; // Inverte a área selecionada
            vizinho.valorObjetivo = avaliarObjetivo(vizinho.areasSelecionadas, retornos);
            if (!estaNaListaTabu(vizinho, listaTabu)) {
                vizinhos.push_back(vizinho);
            }
        }

        // Encontrar o melhor vizinho respeitando a restrição do orçamento
        Solucao melhorVizinho = vizinhos[0];
        for (const Solucao& vizinho : vizinhos) {
            if (vizinho.valorObjetivo > melhorVizinho.valorObjetivo &&
                avaliarObjetivo(vizinho.areasSelecionadas, investimentos) <= orcamento) {
                melhorVizinho = vizinho;
            }
        }

        // Atualizar a lista tabu
        atualizarListaTabu(listaTabu, melhorVizinho);

        // Atualizar a melhor solução
        if (melhorVizinho.valorObjetivo > melhorSolucao.valorObjetivo &&
            avaliarObjetivo(melhorVizinho.areasSelecionadas, investimentos) <= orcamento) {
            melhorSolucao = melhorVizinho;
        }

        solucaoAtual = melhorVizinho;
        iteracao++;
    }

    return melhorSolucao;
}

// Função para realizar busca local
Solucao buscaLocal(const Solucao& solucaoInicial, const vector<int>& investimentos, const vector<int>& retornos, int orcamento) {
    Solucao solucaoAtual = solucaoInicial;
    Solucao melhorSolucao = solucaoInicial;

    while (true) {
        vector<Solucao> vizinhos;
        for (int i = 0; i < solucaoAtual.areasSelecionadas.size(); i++) {
            Solucao vizinho = solucaoAtual;
            vizinho.areasSelecionadas[i] = 1 - vizinho.areasSelecionadas[i]; // Inverte a área selecionada
            vizinho.valorObjetivo = avaliarObjetivo(vizinho.areasSelecionadas, retornos);
            if (avaliarObjetivo(vizinho.areasSelecionadas, investimentos) <= orcamento) {
                vizinhos.push_back(vizinho);
            }
        }

        if (vizinhos.empty()) {
            break;
        }

        Solucao melhorVizinho = vizinhos[0];
        for (const Solucao& vizinho : vizinhos) {
            if (vizinho.valorObjetivo > melhorVizinho.valorObjetivo) {
                melhorVizinho = vizinho;
            }
        }

        if (melhorVizinho.valorObjetivo <= solucaoAtual.valorObjetivo) {
            break;
        }

        solucaoAtual = melhorVizinho;
        if (melhorVizinho.valorObjetivo > melhorSolucao.valorObjetivo) {
            melhorSolucao = melhorVizinho;
        }
    }

    return melhorSolucao;
}

int main() {
    // Dados do problema
    int numAreas = 4;
    vector<int> investimentos = {250000, 350000, 200000, 400000};
    vector<int> retornos = {300000, 150000, 200000, 500000};
    int orcamento = 1000000;

    // Executar busca tabu
    Solucao melhorSolucao = buscaTabu(numAreas, investimentos, retornos, orcamento);

    // Executar busca local a partir da melhor solução encontrada pela busca tabu
    //Solucao solucaoOtima = buscaLocal(melhorSolucao, investimentos, retornos, orcamento);

    // Imprimir a solução encontrada
    cout << "Melhor solução encontrada:" << endl;
    cout << "Áreas selecionadas: ";
    for (int i = 0; i < numAreas; i++) {
        cout << melhorSolucao.areasSelecionadas[i] << " ";
    }
    cout << endl;
    cout << "Valor objetivo: " << melhorSolucao.valorObjetivo << endl;

    return 0;
}
