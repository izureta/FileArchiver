#include "Trie.h"

using NodePtr = std::shared_ptr<Trie::Node>;
using Code = std::vector<bool>;

void Trie::AddCode(Code code, uint16_t symbol) {
    NodePtr node = root;
    for (const bool bit : code) {
        if (node->Child(bit) == nullptr) {
            NodePtr node_ptr(new Node(0, 0, false));
            node->SetChild(bit, node_ptr);
        }
        node = node->Child(bit);
    }
    node->SetSymbol(symbol);
    node->MakeTerminal();
}
