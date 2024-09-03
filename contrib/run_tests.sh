make check &> /dev/null

./test_flat_file
./test_forest
./test_leaf_map

# clean up
rm *.bin
