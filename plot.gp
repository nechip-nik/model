# Установка формата вывода
set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'histograms.png'

# Настройка разделителя для CSV
set datafile separator ','

# Настройка стилей для гистограммы с промежутком
set style data histogram
set style histogram cluster gap 2
set style fill solid 0.7 border
set boxwidth 0.8

# Настройка осей
set xlabel 'Бин (интервал)'
set ylabel 'Частота'
set title 'Гистограммы распределений для разных матриц'

# Настройка диапазона оси Y
set yrange [0:3000]

# Настройка легенды
set key outside right center

# Настройка оси X для лучшей читаемости
set xtics rotate by -45
set xtics font ',10'

# Пропускаем заголовок и используем первый столбец для меток
plot 'output/histogram_1.csv' using 2:xtic(1) skip 1 title 'Moving Matrix' lc rgb '#2ecc71', \
     'output/histogram_2.csv' using 2:xtic(1) skip 1 title 'Sticky Matrix' lc rgb '#e74c3c'

set output