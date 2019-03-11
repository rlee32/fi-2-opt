#pragma once

#include <algorithm>
#include <array>
#include <cstdlib> // abort
#include <iostream>
#include <utility> // swap
#include <vector>

#include "Connection.h"
#include "LengthMap.h"
#include "Move.h"
#include "Segment.h"
#include "constants.h"
#include "primitives.h"

class TourModifier
{
    using Adjacents = std::array<primitives::point_id_t, 2>;
public:
    TourModifier(const std::vector<primitives::point_id_t>& initial_tour
        , const std::vector<primitives::space_t>& x
        , const std::vector<primitives::space_t>& y);

    void move(const Move&, std::vector<Segment>& segments);
    void move(primitives::point_id_t a, primitives::point_id_t b);
    primitives::point_id_t next(primitives::point_id_t i) const { return m_next[i]; }
    std::vector<primitives::point_id_t> order() const;
    primitives::point_id_t size() const { return m_next.size(); }

    const LengthMap& length_map() const { return m_length_map; }

    primitives::length_t length() const;
    primitives::length_t length(primitives::point_id_t i) const;

private:
    LengthMap m_length_map;
    std::vector<primitives::point_id_t> m_order;
    std::vector<Adjacents> m_adjacents;
    std::vector<primitives::point_id_t> m_next;

    void update_next();
    void reorder(const Move&);
    void align_segments(std::vector<Segment>&) const;

    primitives::point_id_t get_other(primitives::point_id_t point, primitives::point_id_t adjacent) const;
    void create_adjacency(const Connection& c);
    void create_adjacency(primitives::point_id_t point1, primitives::point_id_t point2);
    void fill_adjacent(primitives::point_id_t point, primitives::point_id_t new_adjacent);
    void break_adjacency(const Connection& c);
    void break_adjacency(primitives::point_id_t i);
    void break_adjacency(primitives::point_id_t point1, primitives::point_id_t point2);
    void vacate_adjacent_slot(primitives::point_id_t point, primitives::point_id_t adjacent, int slot);
};
