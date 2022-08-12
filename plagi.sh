mkdir solutions/$1
cd solutions/$1
../../split_json_noid.out ../solutions-$1.json
sim_c -p -o ../../$1.simc *.c
cd ../..
./graph.out $1.simc $1.gv $1.list
dot -Tpng $1.gv -o $1.png
./partition.out $1
