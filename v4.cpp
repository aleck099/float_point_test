#include <optional>
#include "jkj/fp/dooly.h"
#include <fast_io.h>

template<std::floating_point F>
class float_reader {
public:
    using decimal_type = jkj::fp::decimal_fp<F, true, false>;
    using carrier_type = decimal_type::carrier_uint;
private:
    using cpchar = const char*;

    struct char_table {
        constexpr char_table() : values{} {}
        bool values[256];
    };

    struct nt1 : char_table {
        constexpr nt1() {
            for (char i = '0'; i <= '9'; ++i)
                this->values[(size_t) i] = true;
            for (char i : { '-', '.' })
                this->values[(size_t) i] = true;
        }
    };

    static constexpr nt1 num_table{};

    static bool is_num_char(char c) {
        return num_table.values[(size_t) c];
    }
public:
    static std::optional<decimal_type> read_decimal(cpchar& p, cpchar end) {
        decimal_type ret;
        while (p != end && !is_num_char(*p)) ++p;
        if (p == end) return std::nullopt;
        if (*p == '-') {
            ret.is_negative = true;
            ++p;
        } else {
            ret.is_negative = false;
        }
        if (p == end) return std::nullopt;
        auto& n = ret.significand;
        n = 0;
        // before point
        for (; p != end && is_num_char(*p); ++p) {
            if (*p == '-') fast_io::throw_parse_code(fast_io::parse_code::invalid);
            if (*p == '.')
                goto AFTER_POINT;
            n *= 10;
            n += *p - '0';
        }
        ret.exponent = 0;
        return ret;
AFTER_POINT:
        ++p;
        auto& ppos = ret.exponent;
        ppos = 0;
        for (; p != end && is_num_char(*p); ++p) {
            if (*p == '-' || *p == '.') fast_io::throw_parse_code(fast_io::parse_code::invalid);
            n *= 10;
            n += *p - '0';
            --ppos;
        }
        return ret;
    }

    static F to_float(decimal_type t) {
        return std::bit_cast<F>(jkj::fp::to_binary_limited_precision(t));
    }
};

using namespace fast_io::io;
using namespace fast_io::mnp;

int main(int argc, char** argv) {
    auto input_filename = os_c_str("numbers.txt");
    if (argc >= 2) input_filename = os_c_str(argv[1]);
    try {
        fast_io::native_file fi{ input_filename, fast_io::open_mode::in | fast_io::open_mode::follow };
        auto fi_stat = status(fi);
        fast_io::native_memory_map_file f{ at(fi), fast_io::file_map_attribute::read_only, fi_stat.size };
        auto p = (const char*) f.data();
        auto pend = (const char*) f.data() + f.size();
        double sum{};
        for (;;) {
            if (p == pend) break;
            auto f = float_reader<double>::read_decimal(p, pend);
            if (f) {
                sum += float_reader<double>::to_float(*f);
            }
        }
        println("Sum: ", fixed(sum));
    } catch (fast_io::error& e) {
        println("IO error: ", e);
        return 1;
    }
    return 0;
}
