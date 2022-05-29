// #include "../catch/catch.hpp"
#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"

int main() {
    std::vector<char> values = {'a', 'b', 'c', 'd', 'e'};
    std::shared_ptr<IBST<char>> tree =
        std::make_shared<CartesianBST<char>>(values.begin(), values.end());
    return 0;
}