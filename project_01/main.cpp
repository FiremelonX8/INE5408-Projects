// Filipe Potrich Cechim (25100483) & Yasmin Ávila Nunes ()

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "array_stack.h"

using namespace std;


class Cenario {
  public:
    Cenario(string& texto, size_t indice_inicial) {
        size_t pos = indice_inicial;
        nome = proxima_tag_conteudo(texto, pos, "nome");
        altura = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "altura") ) );
        largura = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "largura") ) );
        x = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "x") ) );
        y = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "y") ) );
        matriz = matriz_remove_espacos( proxima_tag_conteudo(texto, pos, "matriz") );
        indice_final = pos;
    }
    ~Cenario() {};
    string nome;
    size_t altura;
    size_t largura;
    size_t x;
    size_t y;
    string matriz;
    size_t indice_final;

  private:
    string proxima_tag(string& texto, size_t& pos) {
        string tag = "";
        for ( ; pos < texto.length(); pos++) {
            if (texto[pos] == '<') {
                pos++;
                while (texto[pos] != '>') {
                    tag += texto[pos];
                    pos++;
                }
                pos++;
                return tag;
            }
        }
        return tag;
    }
    string proximo_conteudo(string& texto, size_t& pos) {
        string txt = "";
        while (texto[pos] != '<') {
            txt += texto[pos];
            pos++;
        }
        while (texto[pos] != '>') {
            pos++;
        }
        pos++;
        return txt;
    }
    string proxima_tag_conteudo(string& texto, size_t& pos, string nome_tag) {
        string tag = "";
        while (tag != nome_tag) {
            tag = proxima_tag(texto, pos);
        }
        return proximo_conteudo(texto, pos);
    }
    string matriz_remove_espacos(string texto) {
        string saida;
        for (int i = 0; i < texto.length(); i++) {
            if (texto[i] == '0' || texto[i] == '1') {
                saida += texto[i];
            }
        }
        return saida;
    }
};

// Verifies if the tag is a opening one or a closing one
bool isOpeningTag(string& tagText) {
    if (tagText[0] == '/') {
        return false;
    }
    return true;
}

// Primeiro problema: validação de arquivo XML
// Gets the text and returns the verification for the identified tags
// TODO: REWRITE THIS FUNCTION AND DIVIDE IT INTO MORE CONCISE FUNCTIONS
void verifyTag(string &texto) {
    structures::ArrayStack<string> stackTags = structures::ArrayStack<string>(800);
    bool isOpTag;
    string tagText;

    // For restoring the correct "next iteration" in for loop after while loop
    int contTexto = 0;
    for (int i = 0; i < texto.length(); i++) {
        // cout << texto[i] << endl;
        tagText = "";
        if (texto[i] == '<') {
            // While loop for storing the string that corresponds to the tag name
            contTexto = i;
            while (texto[contTexto] != '>') {
                tagText += texto[contTexto];
                // Adds one unit to each character iterated
                contTexto++;
            }
            // if (texto[i] != '>') {
            //     throw runtime_error("The tag is incorrect:");
            // }
            // i += contTexto;
        }
        // cout << tagText << endl;
        if (tagText.size() > 0) {
            // tagText.erase();
            tagText = tagText.substr(1);
            cout << tagText << endl;
            isOpTag = isOpeningTag(tagText);
            if (isOpTag) {
                stackTags.push(tagText);
            } else {
                // For erasing the '/' character in order to contain the real text 
                // indicating the tag
                tagText = tagText.substr(1);
                if (stackTags.empty()) {
                    throw runtime_error("The hierarchy is incorrect: " + tagText + " is closing without previous opening");
                }
                if (tagText == stackTags.top()) {
                    stackTags.pop();
                } else {
                    throw runtime_error("The hierarchy is incorrect: " + stackTags.top() + " is different from " + tagText);
                }
            }
        }
    }

    if (!stackTags.empty()) {
        throw runtime_error("The hierarchy is incorrect: a tag was not closed");
    }
}

int calculateArea() {
    return 0;
}

/**********************
    FUNÇÃO PRINCIPAL
***********************/
int main() {

    cout << "Iniciou o programa" << endl;
    string filename;

    std::cin >> filename;  // nome do arquivo de entrada 
                           // (no 'executar': escrever pelo teclado;
                           //  no 'avaliar' : nome é passado pelos testes)

    // Abertura do arquivo
    ifstream filexml(filename);
    if (!filexml.is_open()) {
        cerr << "Erro ao abrir o arquivo " << filename << endl;
        throw runtime_error("Erro no arquivo XML");
    }

    cout << "Abriu arquivo" << endl;

    // Leitura do XML completo para 'texto'
    string texto;
    char character;
    while (filexml.get(character)) {
        texto += character;
    }

    cout << "guloso texto" << endl;


    // ----------------------------
    // Sugestão de código para a PARTE 2 do projeto

    // Exemplo de leitura do primeiro cenário - REMOVER ESTAS SAÍDAS DE TELA NA VERSÃO FINAL
    Cenario c1(texto, 0);

    cout << "nome   : " << c1.nome << endl;
    cout << "altura : " << c1.altura << endl;
    cout << "largura: " << c1.largura << endl;
    cout << "x      : " << c1.x << endl;
    cout << "y      : " << c1.y << endl;
    cout << "matriz : " << c1.matriz << endl << endl;

    // Exemplo de leitura do segundo cenário (a partir do índice final de c1) - REMOVER ESTAS SAÍDAS DE TELA NA VERSÃO FINAL
    Cenario c2(texto, c1.indice_final);

    cout << "nome   : " << c2.nome << endl;
    cout << "altura : " << c2.altura << endl;
    cout << "largura: " << c2.largura << endl;
    cout << "x      : " << c2.x << endl;
    cout << "y      : " << c2.y << endl;
    cout << "matriz : " << c2.matriz << endl << endl;

    // Primeiro problema: validação de arquivo XML
    // Verifies if tag is consistent
    // If not, throws errors and does not continue with its execution
    // That is the reason why it is declared as void
    verifyTag(texto);

    return 0;
}
