#include <iostream>
#include <sstream>
#include <string>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "log_duration.h"

using namespace std;

int main()
{
    using namespace transport_catalogue;
    InputReader in_reader;
    TransportCatalogue tc;
    OutputReader out_reader;

    in_reader.requestProcessing(tc, std::cin);
    out_reader.requestProcessing(tc, in_reader, std::cin, std::cout);
    return 0;

    /*ifstream  file("test.txt");

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    using namespace transport_catalogue;
    InputReader in_reader;
    TransportCatalogue tc;
    OutputReader out_reader;
    {
        LOG_DURATION("Time: "s);
        in_reader.requestProcessing(tc, buffer);
        out_reader.requestProcessing(tc, in_reader, buffer, std::cout);
    }
    return 0;*/
}
