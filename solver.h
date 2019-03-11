#pragma once

#include "LengthMap.h"
#include "Move.h"
#include "Swap.h"
#include "Segment.h"
#include "TourModifier.h"
#include "constants.h"
#include "fileio/fileio.h"
#include "primitives.h"
#include "verify.h"

#include <algorithm> // random_shuffle
#include <iterator>
#include <vector>

namespace solver {

inline Move first_improvement(const std::vector<Segment>& segments, const LengthMap& dt)
{
    for (auto s1 = std::crbegin(segments); s1 != std::prev(std::crend(segments)); ++s1)
    {
        for (auto s2 = std::next(s1); s2 != std::crend(segments); ++s2)
        {
            if (not s1->compatible(*s2))
            {
                continue;
            }
            const auto current_length = s1->length + s2->length;
            Segment new_segment_1(s1->a, s2->a, dt);
            auto new_length = new_segment_1.length;
            if (new_length >= current_length)
            {
                continue;
            }
            Segment new_segment_2(s1->b, s2->b, dt);
            new_length += new_segment_2.length;
            if (new_length >= current_length)
            {
                continue;
            }
            const auto improvement = current_length - new_length;
            const auto old_segment_1 = *s1;
            const auto old_segment_2 = *s2;
            return {improvement, {old_segment_1, old_segment_2}, {new_segment_1, new_segment_2}};
        }
    }
    return {};
}

inline Swap first_improvement(const TourModifier& tour)
{
    constexpr primitives::point_id_t start {0};
    for (primitives::point_id_t i {start}; i < tour.size(); ++i)
    {
        const auto first_old_length {tour.length(i)};
        auto j {tour.next(tour.next(i))};
        while (j != start)
        {
            const auto current_length {first_old_length + tour.length(j)};
            auto new_length {tour.length_map().compute_length(i, j)};
            if (new_length > current_length)
            {
                continue;
            }
            new_length += tour.length_map().compute_length(tour.next(i), tour.next(j));
            if (new_length > current_length)
            {
                continue;
            }
            if (new_length < current_length)
            {
                return {i, j, current_length - new_length};
            }
            j = tour.next(j);
        }
    }
    return {};
}

inline void hill_climb(const std::vector<primitives::point_id_t>& ordered_points
    , std::vector<Segment>& segments
    , TourModifier& tour_modifier
    , const std::string save_file_prefix)
{
    auto move = first_improvement(segments, tour_modifier.length_map());
    int iteration{1};
    while (move.improvement > 0)
    {
        tour_modifier.move(move, segments);
        const bool save = iteration % constants::save_period == 0;
        if (save)
        {
            if (segments.size() != ordered_points.size())
            {
                std::cout << __func__ << ": ERROR: tour has become invalid: invalid segment count; actual, expected: "
                    << segments.size() << ", " << ordered_points.size() << std::endl;
                break;
            }
            if (not verify::valid_cycle(segments))
            {
                std::cout << __func__ << ": ERROR: tour has become invalid: invalid cycle.";
                break;
            }
            auto length = verify::tour_length(segments);
            std::cout << "Iteration " << iteration << " tour length: " << length << " (step improvement: " << move.improvement << ")\n";
            fileio::write_ordered_points(tour_modifier.order()
                , "saves/" + save_file_prefix + "_" + std::to_string(length) + ".txt");
        }
        move = first_improvement(segments, tour_modifier.length_map());
        ++iteration;
    }
}

} // namespace solver
