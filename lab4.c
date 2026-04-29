#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>

#define NUM_STATES 5
#define CHAIN_LENGTH 500
#define NUM_CHAINS_FOR_HIST 100
#define BINS 20

// Глобальный генератор случайных чисел
static unsigned long long rng_state = 42;

// Простой генератор случайных чисел (xorshift)
double my_random() {
    rng_state ^= rng_state >> 12;
    rng_state ^= rng_state << 25;
    rng_state ^= rng_state >> 27;
    return (double)(rng_state * 0x2545F4914F6CDD1DULL) / (double)(1ULL << 63);
}

// Получение начального распределения
void get_distribution(double* dist) {
    for (int i = 0; i < NUM_STATES; i++) {
        dist[i] = 1.0 / NUM_STATES;
    }
}

// Получение матриц перехода
void get_matrices(double matrices[2][NUM_STATES][NUM_STATES]) {
    // moving matrix
    double moving[NUM_STATES][NUM_STATES] = {
        {0.00, 0.25, 0.25, 0.25, 0.25},
        {0.25, 0.00, 0.25, 0.25, 0.25},
        {0.25, 0.25, 0.00, 0.25, 0.25},
        {0.25, 0.25, 0.25, 0.00, 0.25},
        {0.25, 0.25, 0.25, 0.25, 0.00}
    };
    
    // sticky matrix
    double sticky[NUM_STATES][NUM_STATES] = {
        {0.6, 0.1, 0.1, 0.1, 0.1},
        {0.1, 0.6, 0.1, 0.1, 0.1},
        {0.1, 0.1, 0.6, 0.1, 0.1},
        {0.1, 0.1, 0.1, 0.6, 0.1},
        {0.1, 0.1, 0.1, 0.1, 0.6}
    };
    
    // Копируем в выходной массив
    for (int i = 0; i < NUM_STATES; i++) {
        for (int j = 0; j < NUM_STATES; j++) {
            matrices[0][i][j] = moving[i][j];
            matrices[1][i][j] = sticky[i][j];
        }
    }
}

// Случайный выбор на основе вероятностей
int random_choice(double* probabilities, int length) {
    double r = my_random();
    double cumulative = 0.0;
    
    for (int i = 0; i < length; i++) {
        cumulative += probabilities[i];
        if (r <= cumulative) {
            return i;
        }
    }
    
    return length - 1;
}

// Генерация одной цепи
void generate_chain(double matrix[NUM_STATES][NUM_STATES], double* distribution, 
                    int* states, double* x_values) {
    // Первое состояние
    states[0] = random_choice(distribution, NUM_STATES);
    x_values[0] = (states[0] + my_random()) / NUM_STATES;
    
    // Остальные состояния
    double probs[NUM_STATES];
    for (int i = 1; i < CHAIN_LENGTH; i++) {
        for (int j = 0; j < NUM_STATES; j++) {
            probs[j] = matrix[states[i - 1]][j];
        }
        states[i] = random_choice(probs, NUM_STATES);
        x_values[i] = (states[i] + my_random()) / NUM_STATES;
    }
}

// Генерация множества цепей
void generate_many_chains(double matrix[NUM_STATES][NUM_STATES], double* distribution,
                          int chains_states[NUM_CHAINS_FOR_HIST][CHAIN_LENGTH],
                          double chains_x[NUM_CHAINS_FOR_HIST][CHAIN_LENGTH],
                          double* all_x, int* all_x_count) {
    *all_x_count = 0;
    
    for (int i = 0; i < NUM_CHAINS_FOR_HIST; i++) {
        generate_chain(matrix, distribution, chains_states[i], chains_x[i]);
        
        // Копируем x значения в общий массив
        for (int j = 0; j < CHAIN_LENGTH; j++) {
            all_x[(*all_x_count)++] = chains_x[i][j];
        }
    }
}

// Построение гистограммы
void build_histogram(double* values, int value_count, int* histogram, int bins) {
    // Инициализация гистограммы
    for (int i = 0; i < bins; i++) {
        histogram[i] = 0;
    }
    
    for (int i = 0; i < value_count; i++) {
        int index = (int)(values[i] * bins);
        if (index == bins) {
            index = bins - 1;
        }
        histogram[index]++;
    }
}

// Печать матрицы
void print_matrix(double matrix[NUM_STATES][NUM_STATES]) {
    for (int i = 0; i < NUM_STATES; i++) {
        for (int j = 0; j < NUM_STATES; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Сохранение цепи в CSV
void save_chain_csv(int* states, double* x_values, const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла: %s\n", path);
        return;
    }
    
    fprintf(file, "step\tstate\tx\n");
    
    for (int i = 0; i < CHAIN_LENGTH; i++) {
        fprintf(file, "%d\t%d\t%.6f\n", i, states[i], x_values[i]);
    }
    
    fclose(file);
}

// Сохранение гистограммы в CSV
void save_histogram_csv(int* histogram, int bins, const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        printf("Ошибка открытия файла: %s\n", path);
        return;
    }
    
    fprintf(file, "bin_start,count\n");
    
    for (int i = 0; i < bins; i++) {
        double bin_start = (double)i / bins;
        fprintf(file, "%.4f,%d\n", bin_start, histogram[i]);
    }
    
    fclose(file);
}

// Создание директории (кросс-платформенная версия)
void create_directory(const char* path) {
#ifdef _WIN32
    _mkdir(path);
#else
    mkdir(path, 0755);
#endif
}

int main() {
    // Инициализация RNG
    rng_state = time(NULL);
    
    // Создание выходной директории
    create_directory("output");
    
    // Получение распределения
    double distribution[NUM_STATES];
    get_distribution(distribution);
    
    // Получение матриц
    double matrices[2][NUM_STATES][NUM_STATES];
    get_matrices(matrices);
    
    printf("Состояний: %d\n", NUM_STATES);
    printf("Длина цепи: %d\n", CHAIN_LENGTH);
    
    // Для хранения всех цепей
    int chains_states[NUM_CHAINS_FOR_HIST][CHAIN_LENGTH];
    double chains_x[NUM_CHAINS_FOR_HIST][CHAIN_LENGTH];
    double all_x[NUM_CHAINS_FOR_HIST * CHAIN_LENGTH];
    int all_x_count;
    
    int matrix_index = 1;
    
    for (int m = 0; m < 2; m++) {
        printf("\nМатрица %d:\n", matrix_index);
        print_matrix(matrices[m]);
        
        // Генерация цепей
        generate_many_chains(matrices[m], distribution, 
                            chains_states, chains_x, 
                            all_x, &all_x_count);
        
        // Построение гистограммы
        int histogram[BINS];
        build_histogram(all_x, all_x_count, histogram, BINS);
        
        // Создание директории для матрицы
        char matrix_dir[100];
        sprintf(matrix_dir, "output/matrix_%d", matrix_index);
        create_directory(matrix_dir);
        
        // Сохранение цепей
        for (int i = 0; i < NUM_CHAINS_FOR_HIST; i++) {
            char path[200];
            sprintf(path, "%s/chain_%d.csv", matrix_dir, i + 1);
            save_chain_csv(chains_states[i], chains_x[i], path);
        }
        
        // Сохранение гистограммы
        char hist_path[200];
        sprintf(hist_path, "output/histogram_%d.csv", matrix_index);
        save_histogram_csv(histogram, BINS, hist_path);
        
        // Вывод гистограммы
        printf("Гистограмма:\n");
        for (int i = 0; i < BINS; i++) {
            printf("%d ", histogram[i]);
        }
        printf("\n");
        
        matrix_index++;
    }
    
    return 0;
}