#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "input_reader.h"
#include "transport_catalogue.h"


namespace transport_catalogue {


class OutputReader
{
public:
    OutputReader(){};
    void requestProcessing(TransportCatalogue& catalogue, InputReader& in_reader, std::istream& input, std::ostream& output);

private:
    void PrintRoutInfo(TransportCatalogue& catalogue, std::string& request, std::ostream& output);
    void PrintStopInfo(TransportCatalogue& catalogue, std::string& request, std::ostream& output);
};


} //end namespace transport_catalogue
