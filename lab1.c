#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_real_combination(int combination[]) {
    int used[21] = {0};
    
    for (int i = 0; i < 4; i++) {
        int num;
        do {
            num = rand() % 20 + 1;
        } while (used[num]);
        
        combination[i] = num;
        used[num] = 1;
    }
}

int count_matches(int player_comb[], int winning_comb[]) {
    int matches = 0;
    int temp_winning[4];
    
    for (int i = 0; i < 4; i++) {
        temp_winning[i] = winning_comb[i];
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (player_comb[i] == temp_winning[j]) {
                matches++;
                temp_winning[j] = -1;
                break;
            }
        }
    }
    
    return matches;
}

int check_jackpot(int player_field1[], int player_field2[], 
                  int winning_field1[], int winning_field2[]) {
    
    int matches1 = count_matches(player_field1, winning_field1);
    int matches2 = count_matches(player_field2, winning_field2);
    
    return (matches1 == 4 && matches2 == 4);
}

int check_four_matches_any_field(int player_field1[], int player_field2[],
                                 int winning_field1[], int winning_field2[]) {
    
    int matches1 = count_matches(player_field1, winning_field1);
    int matches2 = count_matches(player_field2, winning_field2);
    int matches1_cross = count_matches(player_field1, winning_field2);
    int matches2_cross = count_matches(player_field2, winning_field1);
    
    return (matches1 == 4 || matches2 == 4 || 
            matches1_cross == 4 || matches2_cross == 4);
}

int main() {
    srand(time(NULL));

    int player_field1[4] = {5, 7, 12, 15};
    int player_field2[4] = {3, 8, 14, 19};
    
    int winning_field1[4];
    int winning_field2[4];
    
    long long attempts = 0;
    int jackpot_found = 0;
    int four_matches_found = 0;
    
    time_t start_time = time(NULL);
    long long jackpot_attempts = 0;
    long long four_matches_attempts = 0;
    int four_matches_case[4][4];
    
    printf("Поиск джекпота и совпадений 4 чисел в любом поле...\n");
    printf("Ваша комбинация:\n");
    printf("Поле 1: [%d, %d, %d, %d]\n", 
           player_field1[0], player_field1[1], player_field1[2], player_field1[3]);
    printf("Поле 2: [%d, %d, %d, %d]\n\n", 
           player_field2[0], player_field2[1], player_field2[2], player_field2[3]);
    
    while (!jackpot_found || !four_matches_found) {
        attempts++;
        
        generate_real_combination(winning_field1);
        generate_real_combination(winning_field2);
        
        if (!jackpot_found) {
            jackpot_found = check_jackpot(player_field1, player_field2, 
                                         winning_field1, winning_field2);
            if (jackpot_found) {
                jackpot_attempts = attempts;
            }
        }
        

        if (!four_matches_found) {
            if (check_four_matches_any_field(player_field1, player_field2,
                                            winning_field1, winning_field2)) {
                four_matches_found = 1;
                four_matches_attempts = attempts;
                for (int i = 0; i < 4; i++) {
                    four_matches_case[0][i] = winning_field1[i];
                    four_matches_case[1][i] = winning_field2[i];
                }
            }
        }
        
        if (attempts % 1000000 == 0) {
            printf("Попыток: %lld млн\r", attempts/1000000);
            fflush(stdout);
        }
    }
    
    time_t end_time = time(NULL);
    double total_seconds = difftime(end_time, start_time);
    
    printf("\n\n========================================\n");
    printf("         РЕЗУЛЬТАТЫ ПОИСКА\n");
    printf("========================================\n");
    printf("Всего попыток: %lld\n", attempts);
    printf("Время: %.0f сек\n\n", total_seconds);
    
    if (four_matches_found) {
        printf("СЛУЧАЙ 1: СОВПАДЕНИЕ 4 ЧИСЕЛ В ЛЮБОМ ПОЛЕ\n");
        printf("----------------------------------------\n");
        printf("Найдено на попытке: %lld\n", four_matches_attempts);
        printf("\n");
        printf("Ваша комбинация:\n");
        printf("Поле 1: [%d, %d, %d, %d]\n", 
               player_field1[0], player_field1[1], player_field1[2], player_field1[3]);
        printf("Поле 2: [%d, %d, %d, %d]\n", 
               player_field2[0], player_field2[1], player_field2[2], player_field2[3]);
        printf("\n");
        printf("Выигрышная комбинация:\n");
        printf("Поле 1: [%d, %d, %d, %d]\n", 
               four_matches_case[0][0], four_matches_case[0][1], 
               four_matches_case[0][2], four_matches_case[0][3]);
        printf("Поле 2: [%d, %d, %d, %d]\n", 
               four_matches_case[1][0], four_matches_case[1][1], 
               four_matches_case[1][2], four_matches_case[1][3]);

        int m1 = count_matches(player_field1, four_matches_case[0]);
        int m2 = count_matches(player_field2, four_matches_case[1]);
        int m1c = count_matches(player_field1, four_matches_case[1]);
        int m2c = count_matches(player_field2, four_matches_case[0]);
        
        printf("\nСовпадения:\n");
        printf("Поле1 с Поле1: %d\n", m1);
        printf("Поле2 с Поле2: %d\n", m2);
        printf("Поле1 с Поле2: %d\n", m1c);
        printf("Поле2 с Поле1: %d\n", m2c);
        printf("========================================\n\n");
    }
    
    if (jackpot_found) {
        printf("СЛУЧАЙ 2: ДЖЕКПОТ (4+4 совпадения)\n");
        printf("----------------------------------\n");
        printf("Найдено на попытке: %lld\n", jackpot_attempts);
        printf("\n");
        printf("Ваша комбинация:\n");
        printf("Поле 1: [%d, %d, %d, %d]\n", 
               player_field1[0], player_field1[1], player_field1[2], player_field1[3]);
        printf("Поле 2: [%d, %d, %d, %d]\n", 
               player_field2[0], player_field2[1], player_field2[2], player_field2[3]);
        printf("\n");
        printf("Выигрышная комбинация:\n");
        printf("Поле 1: [%d, %d, %d, %d]\n", 
               winning_field1[0], winning_field1[1], winning_field1[2], winning_field1[3]);
        printf("Поле 2: [%d, %d, %d, %d]\n", 
               winning_field2[0], winning_field2[1], winning_field2[2], winning_field2[3]);
        printf("========================================\n");
    }
    
    return 0;
}