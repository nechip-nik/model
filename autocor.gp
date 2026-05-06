# Установка терминала и выходного файла
set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'autocorrelation.png'

# Настройка разделителя для CSV
set datafile separator ','

# Настройка осей
set xlabel 'Лаг (k)'
set ylabel 'Автокорреляция ρ_k'
set title 'Автокорреляционная функция марковских цепей'

# Диапазон осей
set xrange [0:50]
set yrange [-0.3:1.0]

# Сетка
set grid
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"

# Легенда
set key right top

# Построение графиков
plot 'output/autocorrelation_1.csv' using 1:2 skip 1 \
    with linespoints lw 2 lt 1 lc rgb '#2ecc71' pt 7 ps 0.5 \
    title 'Moving Matrix', \
     'output/autocorrelation_2.csv' using 1:2 skip 1 \
    with linespoints lw 2 lt 1 lc rgb '#e74c3c' pt 9 ps 0.5 \
    title 'Sticky Matrix'

set output