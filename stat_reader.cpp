#include "stat_reader.h"

namespace transport_catalogue {


void OutputReader::requestProcessing(TransportCatalogue& catalogue, InputReader& in_reader, std::istream& input, std::ostream& output)
{
    using namespace string_processing;
    int request_num = readLineWithNumber(input);
    std::string request;
    for(int i = 0; i < request_num; ++i)
    {
        getline(input, request);
        RequestType type = in_reader.getRequestType(request);
        if(type == RequestType::BUS)
        {
           PrintRoutInfo(catalogue, request, output);
        }
        else if (type == RequestType::STOP)
        {
           PrintStopInfo(catalogue, request, output);
        }
    }
}

void OutputReader::PrintRoutInfo(TransportCatalogue& catalogue, std::string& request, std::ostream& output)
{
    using namespace string_processing;
    std::string trim_request = trim(request);
    detail::Bus* bus = catalogue.findBus(trim_request);
    output << request + ": ";

    if (bus != nullptr)
    {
        detail::BusInfo bus_info = catalogue.getBusInfo(bus);
        output << std::to_string(bus_info.count_stops) + " stops on route, ";
        output << std::to_string(bus_info.count_unique_stops) + " unique stops, ";
        output << std::to_string(bus_info.real_route_length);
        output << " route length, ";
        output << std::setprecision(6) << bus_info.curvature;
        output << " curvature\n";
        return;
    }
    output << "not found\n";
}

void OutputReader::PrintStopInfo(TransportCatalogue& catalogue, std::string& request, std::ostream& output)
{
    using namespace string_processing;
    std::string trim_request = trim(request);
    detail::Stop* stop = catalogue.findStop(trim_request);
    output << request + ": ";

    if (stop != nullptr)
    {
        detail::StopInfo stop_info = catalogue.getStopInfo(stop);
        if (stop_info.number_buses.empty())
        {
           output << "no buses";
        }
        else
        {
           output << "buses ";
           std::string num_buses = "";
           for(const auto& number: stop_info.number_buses)
           {
               num_buses += number + " ";
           }
           output << trim(num_buses);
        }
        output << "\n";
        return;
    }
    output << "not found\n";
}


} //end namespace transport_catalogue

