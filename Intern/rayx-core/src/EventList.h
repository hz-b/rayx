#pragma once

#include <vector>
#include <span>

#include "Shader/Ray.h"

namespace RAYX {

struct RAYX_API EventList {
    std::vector<Ray> events;
    std::vector<std::span<Ray>> rays;

    static EventList concat(std::vector<EventList>&& lists) {
        const auto numEvents = std::accumulate(lists.begin(), lists.end(), 0, [] (int sum, const auto& events) { return sum + events.size(); });

        auto allEvents = std::vector<Ray>();
        auto allRays = std::vector<std::span<Ray>>();
        allEvents.reserve(numEvents);
        allRays.reserve(numEvents);

        for (auto& list : lists) {
            allEvents.insert(allEvents.end(), list.events.begin(), list.events.end());

            auto rays = std::view::transform(
                list.rays,
                [&] (const auto span) {
                    const auto offset = span.data() - list.events.data();
                    const auto size = span.size();
                    return std::span<Ray>(
                        allEvents.data() + offset,
                        size
                    );
                }
            );
            allRays.insert(allRays.end(), rays.begin(), rays.end());
        }

        return EventList {
            .events = std::move(events),
            .rays = std::move(rays),
        };
    }
};

class RAYX_API EventList {
public:

    const std::vector<Ray>& events() const { return m_events; }

    void push_back(const std::vector<Ray>& ray) {
        m_offsets.push_back(m_events.size());
        m_counts.push_back(ray.size());
        m_events.insert(m_events.end(), ray.begin(), ray.end());
    }

    iterator insert(const_iterator pos, const std::vector<Ray>& ray)

private:
    std::vector<Ray> m_events;
    std::vector<int> m_offsets;
    std::vector<int> m_counts;
};

} // namespace RAYX
