#include <fast_io.h>
#include <fast_io_device.h>
#include <random>

using namespace fast_io::io;
using namespace fast_io::mnp;

int main(int argc, char** argv) {
    auto fn = os_c_str("numbers.txt");
    if (argc >= 2) fn = os_c_str(argv[1]);
    try {
        std::random_device r;
        std::mt19937 g{ r() };
        std::normal_distribution<double> dist{ 0, 10 };

        fast_io::obuf_file f{ fn };
        for (size_t i{}; i != 60000000; ++i) {
            println(f, fixed(dist(g)));
        }
    } catch (fast_io::error& e) {
        println("IO error: ", e);
        return 1;
    }
    return 0;
}

