echo "Running build.sh from gra24cdaproject-g157:"

echo "(1) Compiling..."

# Compiling with 4 threads
make -j4

echo "(1) Compilation done into ./cache_simulation"


echo "(2) Executing..."

# Executing with 4-way associative cache and standard values
./cache_simulation examples/validation/validate.csv --fourway

echo "(2) Execution done"
