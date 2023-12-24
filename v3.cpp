#include <string_view>
#include <cassert>
#include <optional>
#include <cctype>
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

    struct pow_table {
        uint64_t values[20][10];
        constexpr pow_table() {
            // xE0 == 1
            for (size_t n{}; n != 10; ++n) {
                values[0][n] = 1;
            }
            // 0Ex == 0
            for (size_t p{}; p != 20; ++p) {
                values[p][0] = 0;
            }
            uint64_t base{ 1 };
            // 1Ep = 1, 10, 100...
            for (size_t p{}; p != 20; ++p) {
                values[p][1] = base;
                base *= 10;
            }
            // xEp = 1, 2, 3, ..., 10, 20, 30, ...
            for (size_t p{ 1 }; p != 20; ++p) {
                for (size_t x{}; x != 10; ++x) {
                    values[p][x] = values[p][1] * x;
                }
            }
        }
    };

    static constexpr pow_table pt1{};
public:
    static bool is_num_char(char c) {
        return num_table.values[(size_t) c];
    }

    static carrier_type decimal_bit_pow(char n, size_t p) {
        size_t sn = n - '0';
        assert(sn < 10 && sn >= 0);
        assert(p < 20);
        assert(!(sn == p && sn == 0));
        return pt1.values[p][sn];
    }

    static std::optional<decimal_type> read_decimal(cpchar& p, cpchar pend) {
        std::string_view src;
        {
            while (p != pend && !is_num_char(*p)) ++p;
            const char* p_begin = p;
            while (p != pend && is_num_char(*p)) ++p;
            src = { p_begin, p };
        }
        decimal_type ret;
        if (src.empty()) return std::nullopt;
        if (src[0] == '-') {
            ret.is_negative = true;
            src = src.substr(1);
        } else {
            ret.is_negative = false;
        }
        size_t d_bits{};
        auto& exp = ret.exponent;
        exp = 0;
        bool exp_set{};
        bool zero_flag{ true };
        size_t zero_count{};
        for (size_t i{}; i != src.size(); ++i) {
            auto c = src[i];
            if (c == '.') {
                if (exp_set) {
                    fast_io::throw_parse_code(fast_io::parse_code::invalid);
                } else {
                    exp = i - src.size() + 1;
                    exp_set = true;
                }
            } else if (isdigit(c)) {
                if (zero_flag) {
                    if (c == '0') {
                        ++zero_count;
                        continue;
                    } else {
                        zero_flag = false;
                    }
                }
                ++d_bits;
            } else {
                fast_io::throw_parse_code(fast_io::parse_code::invalid);
            }
        }

        auto& bits = ret.significand;
        bits = 0;
        if (zero_flag) return ret;
        --d_bits;
        for (char c : src) {
            if (c == '.') continue;
            if (zero_count) {
                --zero_count;
                continue;
            }
            bits += decimal_bit_pow(c, d_bits);
            --d_bits;
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
        fast_io::native_file_loader f{ input_filename, fast_io::open_mode::follow };
        const char* p = f.data();
        const char* pend = f.data() + f.size();
        double sum{};
        for (;;) {
            auto f = float_reader<double>::read_decimal(p, pend);
            if (f) {
                sum += float_reader<double>::to_float(*f);
            } else {
                break;
            }
        }
        println("Sum: ", fixed(sum));
    } catch (fast_io::error& e) {
        println("IO error: ", e);
        return 1;
    }
    return 0;
}
