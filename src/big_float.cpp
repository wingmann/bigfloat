#include "big_float.h"

#include <utility>

big_float::big_float(int value)
{
    *this = value;
}

big_float::big_float(std::int64_t value)
{
    *this = value;
}

big_float::big_float(double value)
{
    *this = value;
}

big_float::big_float(std::string value)
{
    *this = std::move(value);
}

big_float& big_float::operator=(int value)
{
    *this = std::to_string(value);
    return *this;
}

big_float& big_float::operator=(std::int64_t value)
{
    *this = std::to_string(value);
    return *this;
}

big_float& big_float::operator=(double value)
{
    *this = std::to_string(value);
    return *this;
}

big_float& big_float::operator=(std::string value)
{
    value_.clear();
    decimals_ = {};
    signed_ = false;
    error_ = false;

    std::size_t position{};

    if (value[position] == '-') {
        signed_ = true;
        position++;
    }
    else if (std::isdigit(value[position])) {
        signed_ = false;
    }
    else {
        error_ = true;
        error_code_ = "Unset";
        return *this;
    }
    bool start_dec{};

    while (value[position] != '\0') {
        if (!start_dec) {
            if (value[position] == '.') {
                if (value_.empty()) {
                    signed_ = false;
                    error_ = true;
                    error_code_ = "Unset";
                    return *this;
                }
                start_dec = true;
                position++;
            }
        }

        if (std::isdigit(value[position])) {
            value_.emplace_front(value[position]);
            position++;

            if (start_dec) decimals_++;
        }
        else {
            signed_ = false;
            value_.clear();
            decimals_ = {};
            error_ = true;
            error_code_ = "Unset";
            return *this;
        }
    }
    this->remove_leading_zeros();
    return *this;
}

int big_float::char_to_int(const char& value)
{
    return (value - '0');
}

char big_float::int_to_char(const int& value)
{
    return static_cast<char>(value + '0');
}

int big_float::compare(const big_float& left, const big_float& right)
{
    auto l = (left.value_.size() - left.decimals_);
    auto r = (right.value_.size() - right.decimals_);

    if (l > r)
        return 1;
    else if (l < r)
        return 2;

    if (left.decimals_ > right.decimals_) {
        big_float temp = right;

        while (left.decimals_ > temp.decimals_) {
            temp.decimals_++;
            temp.value_.emplace_front('0');
        }

        for (std::size_t i = left.value_.size() - 1; i >= 0; i--) {
            auto ll = left.value_[i];
            auto t = temp.value_[i];

            if (ll > t)
                return 1;
            else if (ll < t)
                return 2;
        }
    }
    else if (left.decimals_ < right.decimals_) {
        big_float temp = left;

        while (temp.decimals_ < right.decimals_) {
            temp.decimals_++;
            temp.value_.emplace_front('0');
        }

        for (std::size_t i = temp.value_.size() - 1; i >= 0; i--) {
            auto t = temp.value_[i];
            auto rr = right.value_[i];

            if (t > rr)
                return 1;
            else if (t < rr)
                return 2;
        }
    }
    else {
        for (std::size_t i = left.value_.size() - 1; i >= 0; i--) {
            auto ll = left.value_[i];
            auto rr = right.value_[i];

            if (ll > rr)
                return 1;
            else if (ll < rr)
                return 2;
        }
    }
    return {};
}

big_float big_float::add(const big_float& left, const big_float& right)
{
    big_float temp;
    int carry{};

    std::size_t loop_size = left.value_.size() > right.value_.size()
        ? left.value_.size()
        : right.value_.size();

    for (std::size_t i = 0; i < loop_size; ++i) {
        auto first_value = (i > left.value_.size() - 1) ? 0 : char_to_int(left.value_[i]);
        auto second_value = (i > right.value_.size() - 1) ? 0 : char_to_int(right.value_[i]);

        auto aus = first_value + second_value + carry;
        carry = 0;

        if (aus > 9) {
            carry = 1;
            aus -= 10;
        }
        temp.value_.emplace_back(int_to_char(aus));
    }

    if (carry != 0)
        temp.value_.emplace_back(int_to_char(carry));

    return temp;
}

big_float big_float::subtract(const big_float& left, const big_float& right)
{
    big_float temp;

    int carry{};
    int aus;

    for (std::size_t i = 0; i < left.value_.size(); ++i) {
        auto first_value = char_to_int(left.value_[i]);
        auto second_value = (i > right.value_.size() - 1) ? 0 : char_to_int(right.value_[i]);

        first_value -= carry;

        if (first_value < second_value) {
            aus = 10 + first_value - second_value;
            carry = 1;
        }
        else {
            aus = first_value - second_value;
            carry = 0;
        }
        temp.value_.emplace_back(int_to_char(aus));
    }
    return temp;
}

big_float big_float::multiply(const big_float& left, const big_float& right)
{
    big_float res;
    big_float temp;
    int carry{};

    res.value_.emplace_back('0');

    for (std::size_t i = 0; i < right.value_.size(); ++i) {
        for (std::size_t j = 0; j < i; ++j)
            temp.value_.emplace_front('0');

        for (const auto& j : left.value_) {
            auto aus = char_to_int(right.value_[i]) *char_to_int(j) + carry;
            carry = 0;

            while (aus > 9) {
                carry++;
                aus = 10;
            }
            temp.value_.emplace_back(int_to_char(aus));
        }

        if (carry != 0)
            temp.value_.emplace_back(int_to_char(carry));

        carry = 0;
        res = add(res, temp);
        temp.value_.clear();
    }
    return res;
}

big_float operator+(const big_float& left, const big_float& right)
{
    big_float temp;
    big_float first = left;
    big_float second = right;

    if (left.error_ || right.error_)
        return temp;

    if (first.decimals_ > second.decimals_) {
        while (first.decimals_ > second.decimals_) {
            second.decimals_++;
            second.value_.emplace_front('0');
        }
    }
    else if (first.decimals_ < second.decimals_) {
        while (first.decimals_ < second.decimals_) {
            first.decimals_++;
            first.value_.emplace_front('0');
        }
    }

    if (!first.signed_ && second.signed_) {
        auto check = big_float::compare(first, second);

        switch (check) {
        case 1:
        {
            temp = big_float::subtract(first, second);
            temp.signed_ = false;
            temp.decimals_ = first.decimals_;
            temp.remove_leading_zeros();
            temp.error_ = false;
            return temp;
        }
        case 2:
        {
            temp = big_float::subtract(second, first);
            temp.signed_ = true;
            temp.decimals_ = first.decimals_;
            temp.remove_leading_zeros();
            temp.error_ = false;
            return temp;
        }
        default: [[unlikely]]
            temp = 0;
            return temp;
        }
    }
    else if (first.signed_ && !second.signed_) {
        auto check = big_float::compare(first, second);

        switch (check) {
        case 1:
        {
            temp = big_float::subtract(first, second);
            temp.signed_ = true;
            temp.decimals_ = first.decimals_;
            temp.remove_leading_zeros();
            temp.error_ = false;
            return temp;
        }
        case 2:
        {
            temp = big_float::subtract(second, first);
            temp.signed_ = false;
            temp.decimals_ = first.decimals_;
            temp.remove_leading_zeros();
            temp.error_ = false;
            return temp;
        }
        default:
            temp = 0;
            return temp;
        }
    }
    else if (!first.signed_ && !second.signed_) {
        temp = big_float::add(first, second);
        temp.signed_ = false;
        temp.decimals_ = first.decimals_;
        temp.error_ = false;
        return temp;
    }
    else if (first.signed_ && second.signed_) {
        temp = big_float::add(first, second);
        temp.signed_ = true;
        temp.decimals_ = first.decimals_;
        temp.error_ = false;
        return temp;
    }
    return {};
}

big_float operator+(const big_float& left, const int& right)
{
    big_float temp{right};
    return left + temp;
}

big_float operator+(const big_float& left, const double& right)
{
    big_float temp{right};
    return left + temp;
}

big_float operator-(const big_float& left, const big_float& right)
{
    big_float temp;
    big_float first = left;
    big_float second = right;

    if (left.error_ || right.error_)
        return temp;

    if (first.decimals_ > second.decimals_) {
        while (first.decimals_ > second.decimals_) {
            second.decimals_++;
            second.value_.emplace_front('0');
        }
    }
    else if (first.decimals_ < second.decimals_) {
        while (first.decimals_ < second.decimals_) {
            first.decimals_++;
            first.value_.emplace_front('0');
        }
    }

    if (!first.signed_ && right.signed_) {
        temp = big_float::add(first, second);
        temp.signed_ = false;
        temp.decimals_ = first.decimals_;
        temp.error_ = false;
        return temp;
    }
    else if (first.signed_ && !second.signed_) {
        temp = big_float::add(first, second);
        temp.signed_ = true;
        temp.decimals_ = first.decimals_;
        temp.error_ = false;
        return temp;
    }
    else if (!first.signed_ && !second.signed_) {
        // ...
    }
    else if (first.signed_ && second.signed_) {
        // ...
    }
    return {};
}

big_float operator-(const big_float& left, const int& right)
{
    big_float temp{right};
    return left - temp;
}

big_float operator-(const big_float& left, const double& right)
{
    big_float temp{right};
    return left - temp;
}

big_float operator*(const big_float& left, const big_float& right)
{
    return big_float();
}

big_float operator*(const big_float& left, const int& right)
{
    return big_float();
}

big_float operator*(const big_float& left, const double& right)
{
    return big_float();
}

big_float operator/(const big_float& left, const big_float& right)
{
    return big_float();
}

big_float operator/(const big_float& left, const int& right)
{
    return big_float();
}

big_float operator/(const big_float& left, const double& right)
{
    return big_float();
}

big_float big_float::precision_divide(const big_float& left, const big_float& right, int precision)
{
    return big_float();
}

big_float big_float::precision_divide(const big_float& left, const int& right, int precision)
{
    return big_float();
}

big_float big_float::precision_divide(const big_float& left, const double& right, int precision)
{
    return big_float();
}

big_float operator%(const big_float& left, const big_float& right)
{
    return big_float();
}

big_float operator%(const big_float& left, const int& right)
{
    return big_float();
}

const big_float big_float::operator++(int i)
{
    return big_float();
}

big_float& big_float::operator++()
{
    return <#initializer#>;
}

const big_float big_float::operator--(int i)
{
    return big_float();
}

big_float& big_float::operator--()
{
    return <#initializer#>;
}

bool big_float::operator==(const big_float& right) const
{
    return false;
}

bool big_float::operator==(const int& right) const
{
    return false;
}

bool big_float::operator==(const double& right) const
{
    return false;
}

bool big_float::operator!=(const big_float& right) const
{
    return false;
}

bool big_float::operator!=(const int& right) const
{
    return false;
}

bool big_float::operator!=(const double& right) const
{
    return false;
}

bool big_float::operator>(const big_float& right) const
{
    return false;
}

bool big_float::operator>(const int& right) const
{
    return false;
}

bool big_float::operator>(const double& right) const
{
    return false;
}

bool big_float::operator>=(const big_float& right) const
{
    return false;
}

bool big_float::operator>=(const int& right) const
{
    return false;
}

bool big_float::operator>=(const double& right) const
{
    return false;
}

bool big_float::operator<(const big_float& right) const
{
    return false;
}

bool big_float::operator<(const int& right) const
{
    return false;
}

bool big_float::operator<(const double& right) const
{
    return false;
}

bool big_float::operator<=(const big_float& right) const
{
    return false;
}

bool big_float::operator<=(const int& right) const
{
    return false;
}

bool big_float::operator<=(const double& right) const
{
    return false;
}

double big_float::to_double() const
{
    return 0;
}

float big_float::to_float() const
{
    return 0;
}

std::string big_float::to_string() const
{
    return std::string();
}

void big_float::set_precision(int precision)
{

}

void big_float::remove_leading_zeros()
{

}

void big_float::remove_non_significant_zeros()
{

}

bool big_float::has_error() const
{
    return false;
}

std::string big_float::get_error() const
{
    return std::string();
}

int big_float::decimals() const
{
    return 0;
}

std::size_t big_float::ints() const
{
    return 0;
}

std::size_t big_float::memory_size() const
{
    return 0;
}

std::string big_float::expression() const
{
    return std::string();
}
