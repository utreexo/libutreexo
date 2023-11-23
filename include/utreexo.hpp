#include <utreexo.h>
#include <vector>

class UtreexoForest {
private:
  utreexo_forest forest;

public:
  UtreexoForest(char *path) { utreexo_forest_init(&this->forest, path); };
  UtreexoForest() { utreexo_forest_init(&this->forest, "forest.bin"); };
  ~UtreexoForest() { utreexo_forest_free(this->forest); };
  void Modify(std::vector<utreexo_node_hash> utxos,
              std::vector<utreexo_node_hash> stxos) {
    utreexo_forest_modify(this->forest, utxos.data(), utxos.size(),
                          stxos.data(), stxos.size());
  };
};
