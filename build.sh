echo "Running build.sh from gra24cdaproject-g157:"

echo "(1) Compiling..."
make all
echo "(1) Compilation done into ./cache_simulation"

echo "(2) Executing..."
./cache_simulation examples/test.csv
echo "(2) Execution done"