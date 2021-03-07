#pragma once
#include <variant>
#include <cstdint>

typedef std::variant<std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, signed char, unsigned char, float, double, long double> DsData;
