echo "Running build.sh from gra24cdaproject-g157:"

echo "(1) Compiling..."

# Compiling with 4 threads
make -j4

echo "(1) Compilation done into ./cache_simulation"


echo "(2) Executing..."

# Executing our fourwayGeneral validation requests with a 32 byte sized cache
./cache_simulation examples/validation/validateGeneral.csv --cachelines 8 --cacheline-size 4 --directmapped

echo "(2) Execution done"
