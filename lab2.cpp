#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <random>
#include <algorithm>
#include <cassert>
#include <iomanip>

class ConnectedHypergraphGenerator {
private:
    int numVertices;
    int numHyperedges;
    std::vector<std::set<int>> hyperedges;
    std::mt19937 rng;
    
    // Минимальный и максимальный размер ребра
    const int MIN_EDGE_SIZE = 2;
    const int MAX_EDGE_SIZE = 8;
    
public:
    ConnectedHypergraphGenerator(int vertices, int edges, unsigned int seed = std::random_device{}())
        : numVertices(vertices), numHyperedges(edges), rng(seed) {
        // Проверяем, что возможно создать связный гиперграф
        assert(vertices >= 2 && "Должно быть хотя бы 2 вершины");
        assert(edges >= 1 && "Должно быть хотя бы 1 ребро");
        assert(vertices <= edges * MAX_EDGE_SIZE && "Слишком мало рёбер для покрытия всех вершин");
    }
    
    // Основной метод генерации связного гиперграфа
    void generateConnected() {
        hyperedges.clear();
        hyperedges.resize(numHyperedges);
        
        std::cout << "\n=== ГЕНЕРАЦИЯ СВЯЗНОГО ГИПЕРГРАФА ===\n";
        std::cout << "Вершин: " << numVertices << ", рёбер: " << numHyperedges << "\n\n";
        
        // Множество уже использованных вершин
        std::set<int> usedVertices;
        
        // Генерируем рёбра с гарантией связности
        for (int i = 0; i < numHyperedges; ++i) {
            // Определяем размер текущего ребра
            int maxPossibleSize = std::min(MAX_EDGE_SIZE, numVertices);
            int minPossibleSize = (i == 0) ? std::min(MIN_EDGE_SIZE, numVertices) : 2;
            
            std::uniform_int_distribution<> sizeDist(minPossibleSize, maxPossibleSize);
            int edgeSize = sizeDist(rng);
            
            if (i == 0) {
                // ПЕРВОЕ РЕБРО: просто выбираем случайные вершины
                generateFirstEdge(i, edgeSize, usedVertices);
                std::cout << "Ребро #" << (i+1) << " (начальное): размер " << edgeSize 
                          << ", вершины: " << setToString(hyperedges[i]) << "\n";
            } else {
                // ОСТАЛЬНЫЕ РЁБРА: обязательно связаны с предыдущими
                generateConnectedEdge(i, edgeSize, usedVertices);
                std::cout << "Ребро #" << (i+1) << " (связанное): размер " << edgeSize 
                          << ", вершины: " << setToString(hyperedges[i]);
                
                // Показываем через какие вершины связано
                showConnections(i);
            }
        }
        
        // Финальная проверка и достройка
        finalizeCoverage(usedVertices);
        
        // Проверяем связность
        assert(isConnected() && "Гиперграф должен быть связным!");
        std::cout << "\n✓ Гиперграф успешно сгенерирован и является связным!\n";
    }
    
    // Генерация первого ребра
    void generateFirstEdge(int edgeIndex, int size, std::set<int>& usedVertices) {
        std::set<int> newEdge;
        std::uniform_int_distribution<> vertexDist(0, numVertices - 1);
        
        while (newEdge.size() < size) {
            int v = vertexDist(rng);
            newEdge.insert(v);
        }
        
        hyperedges[edgeIndex] = newEdge;
        
        // Обновляем использованные вершины
        for (int v : newEdge) {
            usedVertices.insert(v);
        }
    }
    
    // Генерация связанного ребра
    void generateConnectedEdge(int edgeIndex, int size, std::set<int>& usedVertices) {
        std::set<int> newEdge;
        
        // ШАГ 1: Обязательно добавляем хотя бы одну вершину из предыдущих рёбер
        // (для обеспечения связности)
        std::vector<int> existingVertices(usedVertices.begin(), usedVertices.end());
        std::uniform_int_distribution<> existingDist(0, existingVertices.size() - 1);
        
        int connection_point = existingVertices[existingDist(rng)];
        newEdge.insert(connection_point);
        
        std::cout << "  (связь через вершину " << connection_point << ")\n";
        
        // ШАГ 2: Добавляем новые вершины
        std::vector<int> newVertices;
        for (int v = 0; v < numVertices; ++v) {
            if (usedVertices.find(v) == usedVertices.end()) {
                newVertices.push_back(v);
            }
        }
        
        // Перемешиваем новые вершины для случайности
        std::shuffle(newVertices.begin(), newVertices.end(), rng);
        
        // Добавляем новые вершины, пока не наберём нужный размер
        int newVerticesIndex = 0;
        while (newEdge.size() < size && newVerticesIndex < newVertices.size()) {
            newEdge.insert(newVertices[newVerticesIndex]);
            newVerticesIndex++;
        }
        
        // ШАГ 3: Если всё ещё не хватает вершин, добавляем существующие
        std::shuffle(existingVertices.begin(), existingVertices.end(), rng);
        int existingIndex = 0;
        while (newEdge.size() < size) {
            // Избегаем дубликатов
            if (existingIndex < existingVertices.size()) {
                newEdge.insert(existingVertices[existingIndex]);
                existingIndex++;
            } else {
                // Если все существующие уже использованы, добавляем случайные
                std::uniform_int_distribution<> vertexDist(0, numVertices - 1);
                newEdge.insert(vertexDist(rng));
            }
        }
        
        hyperedges[edgeIndex] = newEdge;
        
        // Обновляем использованные вершины
        for (int v : newEdge) {
            usedVertices.insert(v);
        }
    }
    
    // Финальная достройка - убеждаемся, что все вершины покрыты
    void finalizeCoverage(std::set<int>& usedVertices) {
        std::vector<int> uncovered;
        for (int v = 0; v < numVertices; ++v) {
            if (usedVertices.find(v) == usedVertices.end()) {
                uncovered.push_back(v);
            }
        }
        
        if (!uncovered.empty()) {
            std::cout << "\n! Обнаружены непокрытые вершины: ";
            for (int v : uncovered) std::cout << v << " ";
            std::cout << "\n  Добавляем их в существующие рёбра...\n";
            
            // Распределяем непокрытые вершины по существующим рёбрам
            for (int v : uncovered) {
                std::uniform_int_distribution<> edgeDist(0, numHyperedges - 1);
                int targetEdge = edgeDist(rng);
                hyperedges[targetEdge].insert(v);
                std::cout << "  Вершина " << v << " добавлена в ребро #" << (targetEdge + 1) << "\n";
            }
        }
    }
    
    // Проверка связности гиперграфа
    bool isConnected() {
        if (hyperedges.empty()) return true;
        
        std::set<int> visited;
        std::queue<int> toVisit;
        
        // Начинаем с первой вершины первого ребра
        int startVertex = *hyperedges[0].begin();
        toVisit.push(startVertex);
        visited.insert(startVertex);
        
        // BFS для проверки связности
        while (!toVisit.empty()) {
            int current = toVisit.front();
            toVisit.pop();
            
            // Находим все рёбра, содержащие текущую вершину
            for (const auto& edge : hyperedges) {
                if (edge.find(current) != edge.end()) {
                    // Добавляем все вершины из этого ребра
                    for (int v : edge) {
                        if (visited.find(v) == visited.end()) {
                            visited.insert(v);
                            toVisit.push(v);
                        }
                    }
                }
            }
        }
        
        return visited.size() == numVertices;
    }
    
    // Вспомогательная функция для вывода множества
    std::string setToString(const std::set<int>& s) {
        std::string result = "[";
        bool first = true;
        for (int v : s) {
            if (!first) result += ", ";
            result += std::to_string(v);
            first = false;
        }
        result += "]";
        return result;
    }
    
    // Показать связи ребра с предыдущими
    void showConnections(int edgeIndex) {
        std::set<int> connections;
        for (int i = 0; i < edgeIndex; ++i) {
            for (int v : hyperedges[edgeIndex]) {
                if (hyperedges[i].find(v) != hyperedges[i].end()) {
                    connections.insert(v);
                }
            }
        }
        
        if (!connections.empty()) {
            std::cout << "  (связано через вершины: ";
            for (int v : connections) std::cout << v << " ";
            std::cout << ")\n";
        }
    }
    
public:
    // Сохранение в файл
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        
        file << numVertices << " " << numHyperedges << " # связный гиперграф\n";
        
        int edgeNumber = 1;
        for (const auto& edge : hyperedges) {
            file << "edge" << edgeNumber << " (size=" << edge.size() << "):";
            std::vector<int> sortedVertices(edge.begin(), edge.end());
            std::sort(sortedVertices.begin(), sortedVertices.end());
            for (int v : sortedVertices) {
                file << " " << v;
            }
            file << "\n";
            edgeNumber++;
        }
        
        file.close();
    }
    
    // Сохранение в читаемом формате
    void saveToReadableFile(const std::string& filename) {
        std::ofstream file(filename);
        
        file << "========================================\n";
        file << "СВЯЗНЫЙ ГИПЕРГРАФ\n";
        file << "Вершин: " << numVertices << "\n";
        file << "Рёбер: " << numHyperedges << "\n";
        file << "========================================\n\n";
        
        int edgeNumber = 1;
        for (const auto& edge : hyperedges) {
            file << "РЕБРО #" << edgeNumber << " (размер: " << edge.size() << "):\n";
            file << "  Вершины: ";
            
            std::vector<int> sortedVertices(edge.begin(), edge.end());
            std::sort(sortedVertices.begin(), sortedVertices.end());
            
            for (size_t i = 0; i < sortedVertices.size(); ++i) {
                if (i > 0) file << ", ";
                file << sortedVertices[i];
            }
            file << "\n\n";
            edgeNumber++;
        }
        
        // Добавляем информацию о связности
        file << "========================================\n";
        file << "СТРУКТУРА СВЯЗНОСТИ:\n";
        
        std::vector<std::set<int>> connections(numHyperedges);
        for (int i = 0; i < numHyperedges; ++i) {
            for (int j = i + 1; j < numHyperedges; ++j) {
                std::set<int> intersection;
                for (int v : hyperedges[i]) {
                    if (hyperedges[j].find(v) != hyperedges[j].end()) {
                        intersection.insert(v);
                    }
                }
                if (!intersection.empty()) {
                    file << "Ребро " << (i+1) << " и ребро " << (j+1) 
                         << " пересекаются по вершинам: ";
                    for (int v : intersection) file << v << " ";
                    file << "\n";
                }
            }
        }
        
        file << "========================================\n";
        file.close();
        
    }
    
    // Вывод статистики
    void printStatistics() {
        std::cout << "\n========== СТАТИСТИКА СВЯЗНОГО ГИПЕРГРАФА ==========\n";
        
        // Основные параметры
        std::cout << "Общие параметры:\n";
        std::cout << "  Вершин: " << numVertices << "\n";
        std::cout << "  Рёбер: " << numHyperedges << "\n";
        
        // Статистика по рёбрам
        std::cout << "\nСтатистика по рёбрам:\n";
        std::map<int, int> sizeStats;
        double avgSize = 0;
        int minSize = numVertices;
        int maxSize = 0;
        
        for (const auto& edge : hyperedges) {
            int size = edge.size();
            sizeStats[size]++;
            avgSize += size;
            minSize = std::min(minSize, size);
            maxSize = std::max(maxSize, size);
        }
        avgSize /= numHyperedges;
        
        std::cout << "  Минимальный размер: " << minSize << "\n";
        std::cout << "  Максимальный размер: " << maxSize << "\n";
        std::cout << "  Средний размер: " << std::fixed << std::setprecision(2) << avgSize << "\n";
        std::cout << "  Распределение размеров:\n";
        for (const auto& stat : sizeStats) {
            std::cout << "    Размер " << stat.first << ": " << stat.second << " рёбер (" 
                      << std::fixed << std::setprecision(1) << (100.0 * stat.second / numHyperedges) << "%)\n";
        }
        
        // Статистика по вершинам
        std::cout << "\nСтатистика по вершинам:\n";
        std::vector<int> vertexDegree(numVertices, 0);
        
        for (const auto& edge : hyperedges) {
            for (int v : edge) {
                vertexDegree[v]++;
            }
        }
        
        int minDegree = numHyperedges;
        int maxDegree = 0;
        double avgDegree = 0;
        std::map<int, int> degreeStats;
        
        for (int v = 0; v < numVertices; ++v) {
            int deg = vertexDegree[v];
            degreeStats[deg]++;
            minDegree = std::min(minDegree, deg);
            maxDegree = std::max(maxDegree, deg);
            avgDegree += deg;
        }
        avgDegree /= numVertices;
        
        std::cout << "  Минимальная степень: " << minDegree << "\n";
        std::cout << "  Максимальная степень: " << maxDegree << "\n";
        std::cout << "  Средняя степень: " << std::fixed << std::setprecision(2) << avgDegree << "\n";
        
        // Информация о связности
        std::cout << "\nПроверка связности:\n";
        if (isConnected()) {
            std::cout << "  ✓ ГИПЕРГРАФ СВЯЗНЫЙ\n";
            
            // Считаем количество пересечений между рёбрами
            int totalIntersections = 0;
            for (int i = 0; i < numHyperedges; ++i) {
                for (int j = i + 1; j < numHyperedges; ++j) {
                    std::set<int> intersection;
                    for (int v : hyperedges[i]) {
                        if (hyperedges[j].find(v) != hyperedges[j].end()) {
                            intersection.insert(v);
                        }
                    }
                    if (!intersection.empty()) totalIntersections++;
                }
            }
            std::cout << "  Количество пересекающихся пар рёбер: " << totalIntersections << "\n";
        } else {
            std::cout << "  ✗ ГИПЕРГРАФ НЕ СВЯЗНЫЙ (ошибка!)\n";
        }
        
        std::cout << "================================================\n";
    }
};

int main() {
    // Параметры гиперграфа
    int numVertices = 20;
    int numHyperedges = 6;
    
    std::cout << "Генерация СВЯЗНОГО гиперграфа\n";
    std::cout << "Параметры: " << numVertices << " вершин, " << numHyperedges << " рёбер\n";
    
    // Создаём генератор
    ConnectedHypergraphGenerator generator(numVertices, numHyperedges, 42); // фиксированный seed для воспроизводимости
    
    // Генерируем связный гиперграф
    generator.generateConnected();
    
    // Выводим статистику
    generator.printStatistics();
    
    // Сохраняем в файлы
    generator.saveToFile("connected_hypergraph.txt");
    generator.saveToReadableFile("connected_hypergraph_readable.txt");
    
    std::cout << "\nПрограмма завершена успешно!\n";
    
    return 0;
}