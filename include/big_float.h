#ifndef WINGMANN_BIG_FLOAT_H
#define WINGMANN_BIG_FLOAT_H

#include <iostream>
#include <deque>
#include <cmath>
#include <sstream>
#include <utility>

class big_float {
    bool signed_{};
    std::deque<char> value_;
    int decimals_{};

    // Error checking.
    bool error_{true};
    std::string error_code_{"Unset"};

public:
    big_float() = default;
    explicit big_float(int value);
    explicit big_float(std::int64_t value);
    explicit big_float(double value);
    explicit big_float(std::string value);
    virtual ~big_float() = default;

public:
    big_float& operator=(int value);
    big_float& operator=(std::int64_t value);
    big_float& operator=(double value);
    big_float& operator=(std::string value);

private:
    static int char_to_int(const char& value);
    static char int_to_char(const int& value);

    static int compare(const big_float& left, const big_float& right);

    // Operations without sign and decimals, utilized by operations.
    static big_float add(const big_float& left, const big_float& right);
    static big_float subtract(const big_float& left, const big_float& right);
    static big_float multiply(const big_float& left, const big_float& right);

public:
    // Operations.
    friend big_float operator+(const big_float& left, const big_float& right);
    friend big_float operator+(const big_float& left, const int& right);
    friend big_float operator+(const big_float& left, const double& right);

    friend big_float operator-(const big_float& left, const big_float& right);
    friend big_float operator-(const big_float& left, const int& right);
    friend big_float operator-(const big_float& left, const double& right);

    friend big_float operator*(const big_float& left, const big_float& right);
    friend big_float operator*(const big_float& left, const int& right);
    friend big_float operator*(const big_float& left, const double& right);

    friend big_float operator/(const big_float& left, const big_float& right);
    friend big_float operator/(const big_float& left, const int& right);
    friend big_float operator/(const big_float& left, const double& right);

    static big_float precision_divide(const big_float& left, const big_float& right, int precision);
    static big_float precision_divide(const big_float& left, const int& right,int precision);
    static big_float precision_divide(const big_float& left, const double& right, int precision);

    friend big_float operator%(const big_float& left, const big_float& right);
    friend big_float operator%(const big_float& left, const int& right);

    const big_float operator++(int i);
    big_float& operator++();

    const big_float operator--(int i);
    big_float& operator--();

    // Comparators.
    bool operator==(const big_float& right) const;
    bool operator==(const int& right) const;
    bool operator==(const double& right) const;

    bool operator!=(const big_float& right) const;
    bool operator!=(const int& right) const;
    bool operator!=(const double& right) const;

    bool operator>(const big_float& right) const;
    bool operator>(const int& right) const;
    bool operator>(const double& right) const;

    bool operator>=(const big_float& right) const;
    bool operator>=(const int& right) const;
    bool operator>=(const double& right) const;

    bool operator<(const big_float& right) const;
    bool operator<(const int& right) const;
    bool operator<(const double& right) const;

    bool operator<=(const big_float& right) const;
    bool operator<=(const int& right) const;
    bool operator<=(const double& right) const;

    friend std::ostream& operator<<(std::ostream& out, const big_float& right)
    {
        if (right.error_) {
            out << "NaN";
            return out;
        }
        out << right.signed_;

        for (std::size_t i = right.value_.size() - 1; i >= 0; --i) {
            out << right.value_[i];

            if ((i == right.decimals_) && (i != 0))
                out << '.';
        }
        return out;
    }

    friend std::istream& operator>>(std::istream& in, big_float& right)
    {
        std::string c;
        in >> c;
        right = c;
        return in;
    }

    // Transformation methods.
    [[nodiscard]] double to_double() const;
    [[nodiscard]] float to_float() const;
    [[nodiscard]] std::string to_string() const;

    // Approximate number or increase number decimals_.
    void set_precision(int precision);

    // Remove number leading zeros, utilized by Operations without sign.
    void remove_leading_zeros();

    // Remove number non-significant trailing zeros.
    void remove_non_significant_zeros();

    // Error checking.
    [[nodiscard]] bool has_error() const;
    [[nodiscard]] std::string get_error() const;

    // Miscellaneous methods.
    [[nodiscard]] inline int decimals() const;
    [[nodiscard]] std::size_t ints() const;
    [[nodiscard]] std::size_t memory_size() const;
    [[nodiscard]] std::string expression() const;
};

#endif // WINGMANN_BIG_FLOAT_H
