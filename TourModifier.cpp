#include "TourModifier.h"

TourModifier::TourModifier(const std::vector<primitives::point_id_t>& initial_tour
    , const std::vector<primitives::space_t>& x
    , const std::vector<primitives::space_t>& y)
    : m_length_map(initial_tour, x, y)
    , m_order(initial_tour)
    , m_adjacents(initial_tour.size(), {constants::invalid_point, constants::invalid_point})
    , m_next(initial_tour.size(), constants::invalid_point)
{
    auto prev = initial_tour.back();
    for (auto p : initial_tour)
    {
        create_adjacency(p, prev);
        prev = p;
    }
    update_next();
}

primitives::length_t TourModifier::length() const
{
    primitives::length_t sum {0};
    for (primitives::point_id_t i {0}; i < m_next.size(); ++i)
    {
        sum += m_length_map.length(i, m_next[i]);
    }
    return sum;
}

primitives::length_t TourModifier::length(primitives::point_id_t i) const
{
    return m_length_map.length(i, m_next[i]);
}

std::vector<primitives::point_id_t> TourModifier::order() const
{
    constexpr primitives::point_id_t start {0};
    primitives::point_id_t current {start};
    std::vector<primitives::point_id_t> ordered_points;
    do
    {
        ordered_points.push_back(current);
        current = m_next[current];
    } while (current != start);
    return ordered_points;
}

void TourModifier::move(primitives::point_id_t a, primitives::point_id_t b)
{
    m_length_map.erase(a, m_next[a]);
    m_length_map.erase(b, m_next[b]);
    m_length_map.insert(a, b);
    m_length_map.insert(m_next[a], m_next[b]);
    break_adjacency(a);
    break_adjacency(b);
    create_adjacency(a, b);
    create_adjacency(m_next[a], m_next[b]);
    update_next();
}

void TourModifier::move(const Move& move, std::vector<Segment>& segments)
{
    reorder(move);
    update_next();
    for (auto it {std::begin(segments)}; it != std::end(segments); ++it)
    {
        if (move.old_segments[0] == *it)
        {
            segments.erase(it);
            break;
        }
    }
    for (auto it {std::begin(segments)}; it != std::end(segments); ++it)
    {
        if (move.old_segments[1] == *it)
        {
            segments.erase(it);
            break;
        }
    }
    segments.push_back(move.new_segments[0]);
    segments.push_back(move.new_segments[1]);
    // align after adding / removing segments.
    align_segments(segments);
}

void TourModifier::align_segments(std::vector<Segment>& segments) const
{
    // std::sets only use const iterators, as element values determine order.
    std::vector<Segment> deleted;
    auto it = std::cbegin(segments);
    while (it != std::cend(segments))
    {
        if (it->a == m_next[it->b])
        {
            deleted.push_back(*it);
            std::swap(deleted.back().a, deleted.back().b);
            it = segments.erase(it);
        }
        else
        {
            ++it;
        }

    }
    for (const auto& d : deleted)
    {
        segments.push_back(d);
    }
}

void TourModifier::update_next()
{
    primitives::point_id_t current{0};
    m_next[current] = m_adjacents[current].front();
    do
    {
        auto prev = current;
        current = m_next[current];
        m_next[current] = get_other(current, prev);
    } while (current != 0); // tour cycle condition.
}

void TourModifier::reorder(const Move& move)
{
    // break old segments.
    break_adjacency(move.old_segments[0]);
    break_adjacency(move.old_segments[1]);
    // form new segments.
    create_adjacency(move.new_segments[0]);
    create_adjacency(move.new_segments[1]);
}

primitives::point_id_t TourModifier::get_other(primitives::point_id_t point, primitives::point_id_t adjacent) const
{
    const auto& a = m_adjacents[point];
    if (a.front() == adjacent)
    {
        return a.back();
    }
    else
    {
        return a.front();
    }
}

void TourModifier::create_adjacency(const Connection& c)
{
    create_adjacency(c.a, c.b);
}

void TourModifier::create_adjacency(primitives::point_id_t point1, primitives::point_id_t point2)
{
    fill_adjacent(point1, point2);
    fill_adjacent(point2, point1);
}

void TourModifier::fill_adjacent(primitives::point_id_t point, primitives::point_id_t new_adjacent)
{
    if (m_adjacents[point].front() == constants::invalid_point)
    {
        m_adjacents[point].front() = new_adjacent;
    }
    else if (m_adjacents[point].back() == constants::invalid_point)
    {
        m_adjacents[point].back() = new_adjacent;
    }
    else
    {
        std::cout << "No availble slot for new adjacent." << std::endl;
        std::abort();
    }
}

void TourModifier::break_adjacency(const Connection& c)
{
    break_adjacency(c.a, c.b);
}

void TourModifier::break_adjacency(primitives::point_id_t i)
{
    break_adjacency(i, m_next[i]);
}

void TourModifier::break_adjacency(primitives::point_id_t point1, primitives::point_id_t point2)
{
    vacate_adjacent_slot(point1, point2, 0);
    vacate_adjacent_slot(point1, point2, 1);
    vacate_adjacent_slot(point2, point1, 0);
    vacate_adjacent_slot(point2, point1, 1);
}

void TourModifier::vacate_adjacent_slot(primitives::point_id_t point, primitives::point_id_t adjacent, int slot)
{
    if (m_adjacents[point][slot] == adjacent)
    {
        m_adjacents[point][slot] = constants::invalid_point;
    }
}

