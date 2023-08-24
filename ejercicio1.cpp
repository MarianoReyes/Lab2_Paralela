#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "qsort.c" // Include the sorting function declaration from qsort.c

int main() {
    int N;
    std::cout << "Ingrese la cantidad de números aleatorios: ";
    std::cin >> N;

    // Generar y escribir números aleatorios en el archivo
    std::ofstream outputFile("random_numbers.txt");
    if (!outputFile) {
        std::cerr << "No se pudo abrir el archivo para escritura.\n";
        return 1;
    }

    srand(time(nullptr));
    for (int i = 0; i < N; ++i) {
        int num = rand() % 100; // Generar números entre 0 y 99
        outputFile << num;
        if (i < N - 1) {
            outputFile << ",";
        }
    }
    outputFile.close();

    // Leer los números desde el archivo y almacenarlos en un arreglo en memoria local
    std::ifstream inputFile("random_numbers.txt");
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo para lectura.\n";
        return 1;
    }

    int* numbers = new int[N];
    char comma;
    for (int i = 0; i < N; ++i) {
        inputFile >> numbers[i];
        if (i < N - 1) {
            inputFile >> comma;
        }
    }
    inputFile.close();

    // Usar qsort para clasificar los números
    par_qsort(numbers, 0, N - 1); // Use the sorting function from qsort.c

    // Escribir los números clasificados en un segundo archivo
    std::ofstream sortedOutputFile("sorted_numbers.txt");
    if (!sortedOutputFile) {
        std::cerr << "No se pudo abrir el archivo para escritura.\n";
        delete[] numbers; // Liberar memoria antes de salir
        return 1;
    }

    for (int i = 0; i < N; ++i) {
        sortedOutputFile << numbers[i];
        if (i < N - 1) {
            sortedOutputFile << ",";
        }
    }
    sortedOutputFile.close();

    // Liberar la memoria en el heap
    delete[] numbers;

    std::cout << "Proceso completado. Números aleatorios generados, clasificados y guardados.\n";

    return 0;
}
