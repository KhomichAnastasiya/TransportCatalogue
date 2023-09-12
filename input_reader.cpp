#include "input_reader.h"

namespace transport_catalogue {

namespace string_processing {

std::string readLine(std::istream& input) {
    std::string s;
    getline(input, s);
    return s;
}

int readLineWithNumber(std::istream& input) {
    int result;
    input >> result;
    readLine(input);
    return result;
}

std::string trim(const std::string& str, const std::string& whitespace)
{
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos)
        return "";
    const auto str_end = str.find_last_not_of(whitespace);
    const auto range = str_end - str_begin + 1;
    return str.substr(str_begin, range);
}

} //end namespace string_processing

RequestType InputReader::getRequestType(std::string_view request)
{
    if (request.find("Stop") == 0)
        return RequestType::STOP;
    else if (request.find("Bus") == 0)
        return RequestType::BUS;
    else return RequestType::UNKNOWN;
}

void InputReader::checkDistanceBetwenStops(TransportCatalogue& catalogue, std::string& dist_request, std::string& first_stop_name)
{
    using namespace string_processing;
    detail::Stop* first_stop = catalogue.findStop(first_stop_name);
    bool end = false;
    while (end == false)
    {
        int pos_comma = dist_request.rfind(",");
        std::string substr = "";
        if (pos_comma == -1)
        {
            substr = dist_request;
            end = true;
        }
        else
        {
            substr = dist_request.substr(pos_comma + 1);
            dist_request.erase(pos_comma);
        }
        int pos_to = substr.find("to ");
        std::string second_stop_name = trim(substr.substr(pos_to + 3));
        detail::Stop* second_stop = catalogue.findStop("Stop " + second_stop_name);
        int pos_m = substr.find("m");
        int dist = std::stoi(trim(substr.substr(0, pos_m)));
        catalogue.setDistanceBetwenStops(first_stop, second_stop, dist);
    }
}

void InputReader::parseStopRequest(std::string& request)
{
    using namespace string_processing;
    int pos_colon = request.find(":");
    stop_->name = trim(request.substr(0, pos_colon));
    int pos_comma = request.find(",");
    stop_->latitude = std::stod(trim(request.substr(pos_colon + 1, pos_comma - pos_colon - 1)));
    stop_->longitude = std::stod(trim(request.substr(pos_comma + 1)));
}

void InputReader::parseRouteRequest(TransportCatalogue& catalogue, std::string& request)
{
    using namespace string_processing;
    int pos_colon = request.find(":");
    bus_->name = trim(request.substr(0, pos_colon));
    request.erase(0, pos_colon + 1);
    bool end = false;
    std::string stop_name = "";

    int num_iteration = 0;
    Coordinates coordinates_from;
    Coordinates coordinates_to;
    coordinates_from.lat = -1;
    coordinates_from.lng = -1;
    double geo_dist = 0;

    if (request.find(">") != -1)
    {
        while (end == false)
        {
            int pos = request.find(">");
            if (pos == -1)
            {
                stop_name = trim(request);
                end = true;
            }
            else
                stop_name = trim(request.substr(0, pos));

            detail::Stop* stop = catalogue.findStop("Stop " + stop_name);
            bus_->route.push_back(stop);
            request.erase(0, pos + 1);

            if (num_iteration != 0)
            {
                coordinates_to.lat = stop->latitude;
                coordinates_to.lng = stop->longitude;
                geo_dist += ComputeDistance(coordinates_from, coordinates_to);
            }
            coordinates_from.lat = stop->latitude;
            coordinates_from.lng = stop->longitude;
            ++num_iteration;
        }
    }
    else
    {
        int pos_ = request.rfind("-");
        std::string reverse_direction = request.substr(0, pos_);

        bool reverse_end = false;
        while (end == false)
        {            
            int pos = request.find("-");
            if (pos == -1)
            {
                stop_name = trim(request);
                end = true;
            }
            else
                stop_name = trim(request.substr(0, pos));

            detail::Stop* stop = catalogue.findStop("Stop " + stop_name);
            bus_->route.push_back(stop);
            request.erase(0, pos + 1);

            if (num_iteration != 0)
            {
                coordinates_to.lat = stop->latitude;
                coordinates_to.lng = stop->longitude;
                geo_dist += ComputeDistance(coordinates_from, coordinates_to);
            }
            coordinates_from.lat = stop->latitude;
            coordinates_from.lng = stop->longitude;
            ++num_iteration;
        }

        while (reverse_end == false)
        {
            int reverse_pos = reverse_direction.rfind("-");
            if (reverse_pos == -1)
            {
                stop_name = trim(reverse_direction);
                reverse_end = true;
            }
            else
                stop_name = trim(reverse_direction.substr(reverse_pos + 1));

            detail::Stop* stop = catalogue.findStop("Stop " + stop_name);
            bus_->route.push_back(stop);
            if (reverse_pos != -1)
                reverse_direction.erase(reverse_pos);

            if (num_iteration != 0)
            {
                coordinates_to.lat = stop->latitude;
                coordinates_to.lng = stop->longitude;
                geo_dist += ComputeDistance(coordinates_from, coordinates_to);
            }
            coordinates_from.lat = stop->latitude;
            coordinates_from.lng = stop->longitude;
            ++num_iteration;
        }
    }
    catalogue.setRouteDistGeo(bus_->name, geo_dist);
}

void InputReader::separatingRequests(std::istream& input)
{
    using namespace string_processing;
    int request_num = readLineWithNumber(input);
    std::string request;

    for (int i = 0; i < request_num; ++i)
    {
        getline(input, request);
        RequestType type = getRequestType(request);
        if (type == RequestType::STOP)
        {
            stops_.push_back(std::move(request));
        }
        else if (type == RequestType::BUS)
        {
            buses_.push_back(std::move(request));
        }
        else if (type == RequestType::UNKNOWN)
        {
            continue;
        }
    }
}

void InputReader::requestProcessing(TransportCatalogue& catalogue, std::istream& input)
{
    separatingRequests(input);
    for(std::string& str_stop: stops_)
    {
        stop_ = new detail::Stop();
        parseStopRequest(str_stop);
        catalogue.addStop(stop_);
    }
    for(std::string& str_bus: buses_)
    {
        bus_ = new detail::Bus();
        parseRouteRequest(catalogue, str_bus);
        catalogue.addBus(bus_);
    }

    for(std::string& str_stop: stops_)
    {
        int pos_to = str_stop.find("to ");
        if (pos_to != -1)
        {
            using namespace string_processing;
            int pos_comma = str_stop.find(",");
            int pos_colon = str_stop.find(":");
            std::string stop_name = trim(str_stop.substr(0, pos_colon));
            std::string substr = str_stop.substr(pos_comma+1);
            int pos_comma_substr = substr.find(",");
            std::string dist_request = trim(substr.substr(pos_comma_substr + 1));
            checkDistanceBetwenStops(catalogue, dist_request, stop_name);
        }
    }
}


} //end namespace transport_catalogue


