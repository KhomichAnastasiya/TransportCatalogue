#include "transport_catalogue.h"

namespace transport_catalogue {


void TransportCatalogue::addStop(detail::Stop* stop)
{
    stops_.push_back(std::move(stop));
    stopname_to_stop_[stop->name] = stop;
}

void TransportCatalogue::addBus(detail::Bus* route)
{
    routes_.push_back(std::move(route));
    busname_to_bus_[route->name] = route;
}

detail::Stop* TransportCatalogue::findStop(std::string_view name)
{
	const auto it = stopname_to_stop_.find(name);
    if (it == stopname_to_stop_.end())
        return nullptr;
    return stopname_to_stop_[name];
}

detail::Bus* TransportCatalogue::findBus(std::string_view name)
{
	const auto it = busname_to_bus_.find(name);
    if (it == busname_to_bus_.end())
        return nullptr;
    return busname_to_bus_[name];
}

void TransportCatalogue::setDistanceBetwenStops(const detail::Stop* first_stop, const detail::Stop* second_stop, int distance)
{
    distanse_betwen_stops_[{first_stop, second_stop}] = distance;
}

int TransportCatalogue::getDistanceBetwenStops(const detail::Stop* first_stop, const detail::Stop* second_stop)
{
    if (distanse_betwen_stops_.find({first_stop,second_stop}) != distanse_betwen_stops_.end()){
        return distanse_betwen_stops_.at({first_stop,second_stop});
    }
    else{
        return distanse_betwen_stops_.at({second_stop,first_stop});
    }
}

detail::BusInfo TransportCatalogue::getBusInfo(const detail::Bus* bus)
{
    detail::BusInfo bus_info;

    std::vector<const detail::Stop*> stops;
    stops.reserve(bus->route.size());
    stops.insert(stops.begin(), bus->route.begin(), bus->route.end());
    bus_info.count_stops = static_cast<int>(stops.size());
    std::unordered_set<std::string_view> uniq;
    for(const auto& stop: stops) {
        uniq.insert(std::move(stop->name));
    }

    bus_info.count_unique_stops = uniq.size();

    bus_info.geo_route_length = 0;
    for(int num = 0; num < static_cast<int>(stops.size()) - 1; ++num)
    {
        Coordinates coordinates_from;
        coordinates_from.lat = stops[num]->latitude;
        coordinates_from.lng = stops[num]->longitude;

        Coordinates coordinates_to;
        coordinates_to.lat = stops[num + 1]->latitude;
        coordinates_to.lng = stops[num + 1]->longitude;
        bus_info.geo_route_length += ComputeDistance(coordinates_from, coordinates_to);
    }

    bus_info.real_route_length = 0;
    for(int num = 0; num < static_cast<int>(stops.size()) - 1; ++num)
    {
        bus_info.real_route_length += getDistanceBetwenStops(stops[num], stops[num + 1]);
    }
    bus_info.curvature = bus_info.real_route_length/bus_info.geo_route_length;
    return bus_info;
}

detail::StopInfo TransportCatalogue::getStopInfo(const detail::Stop* stop)
{
    detail::StopInfo stop_info;
    std::set<std::string> passes_through_stop;
    for(auto& [name, bus]: busname_to_bus_)
    {
        std::vector<const detail::Stop*> route = bus->route;
        auto it = std::find(route.begin(), route.end(), stop);
        if (it != route.end())
        {
            passes_through_stop.insert(bus->name);
        }
    }
    stopname_to_busname_[stop->name] = passes_through_stop;

    auto find_stop = stopname_to_busname_.find(stop->name);
    if (find_stop != stopname_to_busname_.end())
    {
        if (!find_stop->second.empty())
        {
            for(auto& bus_name: find_stop->second)
            {
                int pos_space = bus_name.find(" ");
                std::string bus_number = bus_name.substr(pos_space + 1);
                stop_info.number_buses.insert(std::move(bus_number));
            }
        }
    }
    return stop_info;
}


} //end namespace transport_catalogue
