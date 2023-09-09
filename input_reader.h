#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "transport_catalogue.h"

namespace transport_catalogue {

namespace string_processing {

std::string readLine(std::istream& input);
int readLineWithNumber(std::istream& input);
std::string trim(const std::string& str, const std::string& whitespace = " \t");

} //end namespace string_processing

enum class RequestType
{
    STOP,
    BUS,
    UNKNOWN
};

class InputReader
{
public:
    InputReader(){};
    void requestProcessing(TransportCatalogue& catalogue, std::istream& input);
    RequestType getRequestType(std::string_view request);

private:
    detail::Stop* stop_;
    detail::Bus* bus_;
    std::vector<std::string> buses_;
    std::vector<std::string> stops_;

    void parseStopRequest(std::string& request);
    void parseRouteRequest(TransportCatalogue& catalogue, std::string& request);
    void checkDistanceBetwenStops(TransportCatalogue& catalogue, std::string& dist_request, std::string& first_stop_name);
    void separatingRequests(std::istream& input);
};


} //end namespace transport_catalogue
