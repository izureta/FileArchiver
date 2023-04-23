#pragma once
#include <memory>
#include <vector>

class Trie {
public:
    class Node {
    public:
        Node() = default;
        Node(uint16_t symbol, size_t count, bool is_terminal)
        : symbol(symbol), count(count), is_terminal(is_terminal) {}

        using NodePtr = std::shared_ptr<Node>;

        Node(const NodePtr a, const NodePtr b)
        : children{a, b}, is_terminal(false), count(a->count + b->count) {}

        bool IsTerminal() {
            return is_terminal;
        }

        NodePtr Child(size_t num)  {
            if (num < 0 || num >= 2) {
                throw std::invalid_argument("Only 0 or 1 are allowed as a child's number");
            }
            return children[num];
        }

        void SetChild(size_t num, NodePtr child) {
            children[num] = child;
        }

        void SetSymbol(uint16_t new_symbol) {
            symbol = new_symbol;
        }

        void MakeTerminal() {
            is_terminal = true;
        }

        size_t GetCount() {
            return count;
        }

        uint16_t GetSymbol() {
            return symbol;
        }

    private:
        NodePtr children[2] = {nullptr, nullptr};
        bool is_terminal = false;
        uint16_t symbol = 0;
        size_t count = 0;
    };

    using NodePtr = std::shared_ptr<Node>;

    Trie(NodePtr root) : root(root) {}

    NodePtr GetRoot() {
        return root;
    }

    using Code = std::vector<bool>;

    void AddCode(Code code, uint16_t symbol);

private:

    NodePtr root;
};

using NodePtr = std::shared_ptr<Trie::Node>;
