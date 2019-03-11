#include "LengthMap.h"

LengthMap::LengthMap(const LengthCalculator& calculator
    , const std::vector<primitives::point_id_t>& ordered_points)
    : m_calculator(calculator)
    , m_lengths(ordered_points.size())
{
    auto prev {ordered_points.back()};
    for (auto current : ordered_points)
    {
        auto length {m_calculator.length(prev, current)};
        insert(prev, current, length);
        prev = current;
    }
}

void LengthMap::insert(primitives::point_id_t a
    , primitives::point_id_t b
    , primitives::length_t length)
{
    auto& map {m_lengths[std::min(a, b)]};
    map[std::max(a, b)] = length;
}

