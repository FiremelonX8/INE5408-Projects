// Filipe Potrich Cechim (25100483) & Yasmin Ávila Nunes ()

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "array_stack.h"
#include "array_queue.h"

using namespace std;

struct Point {
    int x;
    int y;
};


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
// The following function must return 
void verifyTag(string &texto) {
    structures::ArrayStack<string> stackTags = structures::ArrayStack<string>();
    bool isOpTag;
    string tagText;

    // For restoring the correct "next iteration" in for loop after while loop
    int contTexto = 0;
    for (int i = 0; i < texto.length(); i++) {
        // Reset tagText
        tagText = "";
        if (texto[i] == '<') {
            // While loop for storing the string that corresponds to the tag name
            contTexto = i;
            // While the tag is not over
            while (texto[contTexto] != '>') {
                tagText += texto[contTexto];
                // Adds one unit to each character iterated
                contTexto++;
            }
        }

        // Avoids the tagText to be null when executing the rest of the code
        if (tagText.size() > 0) {
            tagText = tagText.substr(1);

            // cout << tagText << endl;
            
            // Verifies if the tagText represents 
            isOpTag = isOpeningTag(tagText);
            if (isOpTag) {
                stackTags.push(tagText);
            } else {
                // For erasing the '/' character in order to contain the real text indicating the tag
                tagText = tagText.substr(1);
                
                // Imply runtime error if stack is empty and it is a closing tag
                if (stackTags.empty()) {
                    throw runtime_error("The hierarchy is incorrect: " + tagText + " is closing without previous opening");
                }
                
                // Pops top if tag is a closing one and the top is an opening equal to it
                if (tagText == stackTags.top()) {
                    stackTags.pop();
                } else {
                    throw runtime_error("The hierarchy is incorrect: " + stackTags.top() + " is different from " + tagText);
                }
            }
        }
    }
    
    // The tag was not closed until the end of the file (needs to be optimized though)
    if (!stackTags.empty()) {
        throw runtime_error("The hierarchy is incorrect: a tag was not closed");
    }
}

// Set all matrix's elements to zero
void resetMatrix(char** matrix, size_t altura, size_t largura) {
    for (size_t i = 0; i < altura; i++) {
        matrix[i] = new char[largura];
        for (size_t j = 0; j < largura; j++) {
            matrix[i][j] = '0';
        }
    }
}

// Delete matrix pointers (deallocated dynamic structures)
void destroyMatrix(char** matrix, size_t altura, size_t largura) {
    for (int i = 0; i < altura; i++) {
        delete [] matrix[i];
    }
    delete [] matrix;
}

// Converts the matrix from the string matrix from XML to char double pointer
void convertMatrix(string matrixStr, char** matrixE, size_t altura, size_t largura) {
    size_t strIndex = 0;
    for (size_t i = 0; i < altura; ++i) {
        for (size_t j = 0; j < largura; ++j) {
            matrixE[i][j] = matrixStr[strIndex++];
        }
    }
}

// void verifyNeighbourhood() {
    
// }

// Verifies the correctness of the paths ->
// NO negative coordinate;
// NO trespassing the limits of the matrix.
void verifyPaths(Point p, size_t altura, size_t largura, structures::ArrayQueue<Point>& queue, char** matrixR, char** matrixE, int& area) {
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int nx = p.x + dx[i];
        int ny = p.y + dy[i];

        // Ensure we check against altura and largura specifically
        if (nx >= 0 && nx < static_cast<int>(altura) && 
            ny >= 0 && ny < static_cast<int>(largura) &&
            matrixR[nx][ny] == '0' && matrixE[nx][ny] == '1') {
            
            matrixR[nx][ny] = '1'; 
            area++;
            queue.enqueue({nx, ny});
        }
    }
}

// Calculates the area that the robot is responsible for cleaning
// Segundo problema: determinação de área do espaço que o robô deve limpar
int calculateArea(char** matrixR, char** matrixE, size_t altura, size_t largura, Point start) {
    int area = 1;
    // Use a safe capacity for the queue
    structures::ArrayQueue<Point> queue(altura * largura);
    
    matrixR[start.x][start.y] = '1';
    queue.enqueue(start);

    while (!queue.empty()) {
        Point current = queue.dequeue();
        // Update verifyPaths to accept altura/largura instead of a single size
        verifyPaths(current, altura, largura, queue, matrixR, matrixE, area);
    }
    return area;
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

    // Leitura do XML completo para 'texto'
    string texto;
    char character;
    while (filexml.get(character)) {
        texto += character;
    }

    //
    //
    //
    // END OF STATIC SECTION
    //
    //
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

    // OUR CODE / Initializing variables
    //
    //
    //
    //
    //
    //
    int area;
    Point robotStart = {(int)c1.x, (int)c1.y};

    const size_t size_matrix0 = c1.matriz.length();
    char** matrixR;
    char** matrixE;
    matrixR = new char*[c1.altura];
    matrixE = new char*[c1.altura];
    for (size_t i = 0; i < c1.altura; ++i) {
        matrixE[i] = new char[c1.largura];
    }
    
    // Primeiro problema: validação de arquivo XML
    // Verifies if tag is consistent
    // If not, throws errors and does not continue with its execution
    // That is the reason why it is declared as void
    verifyTag(texto);
    
    // Initialize matrix to zero
    resetMatrix(matrixR, c1.altura, c1.largura);
    
    // Converts the matrix in string to a matrix in chars an allocate a pointer
    convertMatrix(c1.matriz, matrixE, c1.altura, c1.largura);

    // Calculates area based on the input matrix
    area = calculateArea(matrixR, matrixE, c1.altura, c1.largura, robotStart);

    // Prints the area that the robot is responsible for cleaning
    cout << area << "m²" << endl;

    // Deallocate the matrix's pointers
    destroyMatrix(matrixR, c1.altura, c1.largura);
    destroyMatrix(matrixE, c1.altura, c1.largura);

    return 0;
}
