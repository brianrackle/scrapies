#include <future>
#include <regex>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
//#include "gumbo.h"
#include "curl_ops.h"

std::string get_symbol(char delimiter, size_t pos, std::string line)
{
    auto delimiter_it = std::find_if(line.begin(), line.end(), [&](const auto & v)
    {
        if(v == delimiter)
        {
            if(pos == 0)
            {
                return true;
            }
            else
            {
                --pos;
            }
        }
        return false;
    });
    
    std::regex symbol_regex(R"EOF((\w+))EOF");
    std::smatch symbol_match;
    std::regex_search(std::string(line.begin(), delimiter_it), symbol_match, symbol_regex);
    return symbol_match[1];
}

int main(int argc, char ** argv)
{
    //https://gist.github.com/marijn/396531
    //read symbols from input file
    //lookup symbols using http://www.wikinvest.com/wiki/Aapl formatting
    //navigate to sec filing and pull relevant information EIN, Address, phone number
    //use google https://developers.google.com/maps/documentation/geocoding/#Limits
    //2500 requests per 24 hour period.
    //5 requests per second.
    //use (Address of principal executive offices) to identify potential addresses to send to geocoder
    //only need to look for city, state, country
    //start by identifying country and then parsing backwards not everything has a state

    //read by line, regex using '|' delimiter, first value is stock symbol

//    GumboOutput* output = gumbo_parse("<h1>Hello World!</h1>");
//    gumbo_destroy_output(&kGumboDefaultOptions, output);
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;
    namespace crl = scrapies_curl;

    std::string command_file_option = "command-file";
    std::string data_file_option = "data-file";

//    po::options_description desc("Allowed options");
//    desc.add_options()
//            ("help", "produce help message")
//            (command_file_option.c_str(), po::value<fs::path>(), "gnu plot command file")
//            (data_file_option.c_str(), po::value<fs::path>(), "gnu plot command file");
//
//    po::positional_options_description p_desc;
//    p_desc.add(command_file_option.c_str(), 1).add(data_file_option.c_str(), 1);
//
//    po::variables_map vm;
//    po::store(po::command_line_parser(argc, argv).options(desc).positional(p_desc).run(), vm);
//    po::notify(vm);

//    if (argc <= 1 || vm.count("help")) {
//        std::cout << desc << "\n";
//        return EXIT_SUCCESS;
//    }
//
//    fs::path command_file_path;
//    if (vm.count(command_file_option))
//        command_file_path = vm[command_file_option].as<fs::path>();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    fs::path nasdaq_file = "nasdaq_symbols";
    fs::path other_file = "other_symbols";
    
    //TODO(brian): config file should hold files to retrieve
    std::ifstream nasdaq_symbols_file(crl::get_ftp(nasdaq_file.c_str(), "ftp://ftp.nasdaqtrader.com/symboldirectory/nasdaqlisted.txt"));
    std::ifstream other_symbols_file(crl::get_ftp(other_file.c_str(), "ftp://ftp.nasdaqtrader.com/symboldirectory/otherlisted.txt"));
    
    std::vector<std::string> symbols;
    std::string line;
    while(std::getline(nasdaq_symbols_file, line))
    {
         symbols.push_back(get_symbol('|', 0, line));
    }
    symbols.pop_back();

    while(std::getline(other_symbols_file, line))
    {
         symbols.push_back(get_symbol('|', 0, line));
    }
    symbols.pop_back();
    
    std::sort(symbols.begin(), symbols.end());
    symbols.resize(symbols.size() - std::distance(std::unique(symbols.begin(), symbols.end()), symbols.end()));
    symbols.shrink_to_fit();
    
    for(const auto & v : symbols)
    {
        printf("%s\n", v.c_str());
    }
    //get symbol website from wikinvest
    //http://www.wikinvest.com/wiki/SYMBOL
    
    return 0;
}