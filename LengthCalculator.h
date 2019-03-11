#pragma once

#include "primitives.h"

#include <algorithm> // min, max
#include <cmath> // sqrt
#include <vector>

class LengthCalculator
{
public:
    LengthCalculator(const std::vector<primitives::space_t>& x, const std::vector<primitives::space_t>& y)
        : m_x(x), m_y(y) {}

    primitives::length_t length(primitives::point_id_t a, primitives::point_id_t b) const
    {
        auto dx = m_x[a] - m_x[b];
        auto dy = m_y[a] - m_y[b];
        auto exact = std::sqrt(dx * dx + dy * dy);
        return exact + 0.5; // return type cast.
    }

    const primitives::space_t& x(primitives::point_id_t i) const { return m_x[i]; }
    const primitives::space_t& y(primitives::point_id_t i) const { return m_y[i]; }

private:
    const std::vector<primitives::space_t>& m_x;
    const std::vector<primitives::space_t>& m_y;

};
