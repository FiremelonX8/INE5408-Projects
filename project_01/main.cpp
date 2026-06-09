// Filipe Potrich Cechim (25100483) & Yasmin Ávila Nunes (24100515)

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
                    cout << "erro" << endl;
                    exit(0);
                }
                
                // Pops top if tag is a closing one and the top is an opening equal to it
                if (tagText == stackTags.top()) {
                    stackTags.pop();
                } else {
                    cout << "erro" << endl;
                    exit(0);
                }
            }
        }
    }
    
    // The tag was not closed until the end of the file (needs to be optimized though)
    if (!stackTags.empty()) {
        cout << "erro" << endl;
        exit(0);
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
    
    if (matrixE[start.x][start.y] == '0') {
        return 0;
    }
    
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

    std::cin >> filename;  // XML input file name

    // Opening XML file
    ifstream filexml(filename);

    if (!filexml.is_open()) {
        cerr << "Erro ao abrir o arquivo " << filename << endl;
        throw runtime_error("Erro no arquivo XML");
    }

    // Reads the entire XML file into the string "texto"
    string texto;
    char character;

    while (filexml.get(character)) {
        texto += character;
    }

    // Primeiro problema:
    // Verifies if XML tags are correctly nested
    verifyTag(texto);

    // Variable responsible for controlling
    // the reading position inside the XML
    size_t pos = 0;

    // Iterates through all scenarios in the XML
    while (texto.find("<cenario>", pos) != string::npos) {

        // Creates current scenario
        Cenario currentScenario(texto, pos);

        // Updates the reading position
        pos = currentScenario.indice_final;

        // Stores robot initial position
        Point robotStart = {
            (int)currentScenario.x,
            (int)currentScenario.y
        };

        // Dynamic allocation of matrices
        char** matrixR;
        char** matrixE;

        matrixR = new char*[currentScenario.altura];
        matrixE = new char*[currentScenario.altura];

        // Allocates rows for matrixE
        for (size_t i = 0; i < currentScenario.altura; ++i) {
            matrixE[i] = new char[currentScenario.largura];
        }

        // Initializes reconstruction matrix with zeros
        resetMatrix(
            matrixR,
            currentScenario.altura,
            currentScenario.largura
        );

        // Converts matrix string into bidimensional matrix
        convertMatrix(
            currentScenario.matriz,
            matrixE,
            currentScenario.altura,
            currentScenario.largura
        );

        // Calculates connected component area
        int area = calculateArea(
            matrixR,
            matrixE,
            currentScenario.altura,
            currentScenario.largura,
            robotStart
        );

        // Prints robot cleaning area
        cout << currentScenario.nome
            << " "
            << area
            << endl;

        // Deallocates matrices
        destroyMatrix(
            matrixR,
            currentScenario.altura,
            currentScenario.largura
        );

        destroyMatrix(
            matrixE,
            currentScenario.altura,
            currentScenario.largura
        );
    }

    return 0;
}