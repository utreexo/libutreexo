// #include "src/flat_file.h"
#include "src/map_forest.h"

int main() {
  struct utreexo_forest_file *file = NULL;
  utreexo_forest_file_init(&file, "forest.bin");

  struct utreexo_forest p = {
      .data = file,
      .roots = {0},
      .nLeaf = 0,
  };
  utreexo_forest_add(&p, (utreexo_node_hash){
                             .hash = {0x00, 0x01, 0x02, 0x03},
                         });
  utreexo_forest_add(&p, (utreexo_node_hash){
                             .hash = {0x00, 0x01, 0x02, 0x03},
                         });
  utreexo_forest_add(&p, (utreexo_node_hash){
                             .hash = {0x00, 0x01, 0x02, 0x03},
                         });
  utreexo_forest_add(&p, (utreexo_node_hash){
                             .hash = {0x00, 0x01, 0x02, 0x03},
                         });
  return 0;
}