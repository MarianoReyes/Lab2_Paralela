#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include "qsort.c" // Include the sorting function declaration from qsort.c
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

int main()
{
    clock_t start_time = clock();
    int N;
    cout << "Ingrese la cantidad de números aleatorios: ";
    cin >> N;

    // Generar y escribir números aleatorios en el archivo
    ofstream outputFile("random_numbers.txt");
    if (!outputFile)
    {
        cerr << "No se pudo abrir el archivo para escritura.\n";
        return 1;
    }

    srand(time(nullptr));
    int *numbersG = new int[N];

    double start_time_parallel = omp_get_wtime();
#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < N; ++i)
        {
            numbersG[i] = rand() % 100; // Generar números entre 0 y 99
        }

// Nodo maestro escribe en el archivo
#pragma omp master
        {
            for (int i = 0; i < N; ++i)
            {
                outputFile << numbersG[i];
                if (i < N - 1)
                {
                    outputFile << ",";
                }
            }
            outputFile.close();
        }
    }
#pragma omp barrier // Sincronizar antes de proceder
    outputFile.close();
    double end_time_parallel = omp_get_wtime();

    // Leer los números desde el archivo y almacenarlos en un arreglo en memoria local
    ifstream inputFile("random_numbers.txt");
    if (!inputFile)
    {
        cerr << "No se pudo abrir el archivo para lectura.\n";
        return 1;
    }

    int *numbers = new int[N];
    char comma;

    double start_time_parallel2 = omp_get_wtime();
#pragma omp parallel for // private(comma)
    for (int i = 0; i < N - 1; ++i)
    {
#pragma omp critical
        {
            inputFile >> numbers[i];
            inputFile >> comma;
        }
    }
    double end_time_parallel2 = omp_get_wtime();
    inputFile >> numbers[N - 1];
    inputFile.close();

    // Usar qsort para clasificar los números
    par_qsort(numbers, 0, N - 1);

    // Escribir los números clasificados en un segundo archivo
    ofstream sortedOutputFile("sorted_numbers.txt");
    if (!sortedOutputFile)
    {
        cerr << "No se pudo abrir el archivo para escritura.\n";
        delete[] numbers; // Liberar memoria antes de salir
        return 1;
    }
    std::vector<std::string> parallelOutput(N); // Vector para almacenar las salidas de cada hilo

    double start_time_parallel3 = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        std::stringstream ss;
        ss << numbers[i];
        if (i < N - 1)
        {
            ss << ',';
        }
        parallelOutput[i] = ss.str(); // Cada hilo escribe su parte en el vector
    }

// Un solo "master thread" se encarga de la escritura
#pragma omp parallel
    {
#pragma omp master
        {
            for (const auto &str : parallelOutput)
            {
                sortedOutputFile << str;
            }
            sortedOutputFile.close();
        }
    }
    double end_time_parallel3 = omp_get_wtime();

    // Liberar la memoria en el heap
    delete[] numbers;

    cout << "Proceso completado. Números aleatorios generados, clasificados y guardados.\n";
    // Fin del cronómetro
    clock_t end_time = clock();
    double elapsed_time_parallel = end_time_parallel - start_time_parallel + end_time_parallel2 - start_time_parallel2 + end_time_parallel3 - start_time_parallel3;
    double elapsed_time = double(end_time - start_time - elapsed_time_parallel) / CLOCKS_PER_SEC;
    cout << "Tiempo de ejecución: " << elapsed_time << " segundos.\n";
    cout << "Tiempo de ejecución paralela: " << elapsed_time_parallel << " segundos.\n";
    double speedup = elapsed_time / elapsed_time_parallel;
    cout << "Speedup: " << speedup << endl;

    return 0;
}
