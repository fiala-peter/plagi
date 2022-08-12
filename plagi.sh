mkdir solutions/$1
cd solutions/$1
../../split_json ../solutions-$1.json
sim_c -p -o $1.simc *.c
../../graph $1.simc $1.gv
dot -Tpng $1.gv -o $1.png
../../partition $1
