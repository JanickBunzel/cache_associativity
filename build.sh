echo "Running build.sh from gra24cdaproject-g157:"

echo "(1) Compiling..."

# Running make with 4 threads
make -j4

echo "(1) Compilation done into ./cache_simulation"


echo "(2) Executing..."

./cache_simulation examples/validation/validate.csv --fourway

echo "(2) Execution done"
