#pragma once

#include "LengthCalculator.h"
#include "primitives.h"

#include <unordered_map>
#include <vector>

class LengthMap
{
public:
    LengthMap(const LengthCalculator&, const std::vector<primitives::point_id_t>& ordered_points);

    void insert(primitives::point_id_t a
        , primitives::point_id_t b
        , primitives::length_t length);

private:
    const LengthCalculator& m_calculator;
    std::vector<std::unordered_map<primitives::point_id_t, primitives::length_t>> m_lengths;

};
