#include <emscripten/bind.h>
#include <type_traits>
#include <iostream>

class Test {
public:
    Test(int x) : x_(x) {}
    int getX() const { return x_; }
private:
    int x_;
};

EMSCRIPTEN_BINDINGS(test) {
    emscripten::class_<Test>("Test")
        .constructor<int>()
        .function("getX", &Test::getX);
}

int main() {
    std::cout << "Test" << std::endl;
    return 0;
}