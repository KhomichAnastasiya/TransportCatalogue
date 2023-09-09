#pragma once

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "geo.h"

namespace transport_catalogue {


namespace detail {

struct Stop {
	std::string name;
    double latitude;
    double longitude;

    bool operator==(const Stop& other) const {
        return name == other.name && latitude == other.latitude && longitude == other.longitude;
    }
    bool operator!=(const Stop& other) const {
        return !(*this == other);
    }
};

struct Bus {
    std::string name;
    std::vector<const Stop*> route;
};

struct BusInfo {
    int count_stops;
    int count_unique_stops;
    double geo_route_length;
    int real_route_length;
    double curvature;
};

struct StopInfo {
    std::set<std::string> number_buses;
};

template<typename T>
struct PairHasher {
public:
    size_t operator()(const std::pair<T, T> pair) const {
        return (hasher(pair.first) + hasher(pair.second));
    }
    std::hash<T> hasher;
};

} //end namespace detail

class TransportCatalogue
{
public:
    void addStop(detail::Stop* stop);
    void addBus(detail::Bus* route);
    detail::Stop* findStop(std::string_view name);
    detail::Bus* findBus(std::string_view name);
    detail::BusInfo getBusInfo(const detail::Bus* bus);
    detail::StopInfo getStopInfo(const detail::Stop* stop);
    void setDistanceBetwenStops(const detail::Stop* first_stop, const detail::Stop* second_stop, int distance);
    int getDistanceBetwenStops(const detail::Stop* first_stop, const detail::Stop* second_stop);

private:
    std::deque<detail::Stop*> stops_;
    std::deque<detail::Bus*> routes_;
    std::unordered_map<std::string_view, detail::Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, detail::Bus*> busname_to_bus_;
    std::unordered_map<std::string, std::set<std::string>> stopname_to_busname_;
    std::unordered_map<std::pair<const detail::Stop*,const detail::Stop*>, int, detail::PairHasher<const detail::Stop*>> distanse_betwen_stops_;
};


} //end namespace transport_catalogue
