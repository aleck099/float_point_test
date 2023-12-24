#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    std::string_view fn{ "numbers.txt" };
    if (argc >= 2) fn = argv[1];
    std::ifstream fin{ std::string{ fn } };
    double sum{};
    for (;;) {
        double d;
        fin >> d;
        if (!fin)
            break;
        sum += d;
    }
    std::cout << "Sum: " << sum << '\n';
    return 0;
}
