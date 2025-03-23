#include "../../include/serene.hpp"
#include <memory>

int main() {
    using namespace serene;
    relaxed_graph* graph = new relaxed_graph();
    delete graph;
    
    {
        auto graph = std::make_unique<relaxed_graph>();
    }
    return 0;
}