# Установка формата вывода
set terminal pngcairo size 1200,800 enhanced font 'Arial,12'
set output 'output/sticky_matrix.png'

# Настройка разделителя для CSV
set datafile separator '\t'

# Создание layout с двумя графиками вертикально
set multiplot layout 2,1 title 'Sticky Matrix - Цепь Маркова' font ',14'

# График 1: Sticky Matrix - Состояния
set title 'Состояния (States)'
set xlabel 'Шаг (step)'
set ylabel 'Состояние'
set yrange [-0.5:4.5]
set ytics 0,1,4
set grid
plot 'output/matrix_2/chain_1.csv' using 1:2 with lines lw 2 lc rgb '#e74c3c' title 'Состояние'

# График 2: Sticky Matrix - X значения
set title 'X значения'
set xlabel 'Шаг (step)'
set ylabel 'x'
set yrange [0:1]
set grid
plot 'output/matrix_2/chain_1.csv' using 1:3 with lines lw 2 lc rgb '#f39c12' title 'x'

unset multiplot
set output