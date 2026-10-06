// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A2619B6
// Struct for each entry with all data
// 2 Priority queues (1 for sellers, 1 for buyers)

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <getopt.h>
#include "Info.hpp"

using namespace std;

// Everything the command line can change about a run. The defaults here are
// the spec's defaults, so an option that is never given just keeps its value.
struct Options {
    bool verbose = false;
    bool median = false;
    bool trader_info = false;
    bool time_travelers = false;
};  // Options{}


// Print help for the user when requested.
// argv[0] is the name of the currently executing program
void printHelp(char *command) {
    cout << "Usage: " << command << " [options] < inputfile\n";
    cout << "Options:\n";
    cout << "  -h, --help                     Print this message and exit\n";
    cout << "  -v, --verbose                  Print every trade as it happens\n";
    cout << "  -m, --median                   Print the current median match price for all stocks\n";
    cout << "  -i, --trader_info              Print trader ID, number bought and sold, and net value\n";
    cout << "  -t, --time_travelers           Print ideal times a time traveler should've bought and sold stocks to make maximum profit\n";
}  // printHelp()

// Member functions if needed

// Process the command line; there is no return value, but the Options
// struct is passed by reference and is modified by this function to send
// information back to the calling function.
void getOptions(int argc, char **argv, Options &options) {
    // These are used with getopt_long()
    opterr = static_cast<int>(false);  // Let us handle all error output for command line options
    int choice = 0;
    int index = 0;

    // NOLINTBEGIN: getopt predates C++ style, this usage is from `man getopt`
    option longOptions[] = {
        {"help",           no_argument, nullptr, 'h'},
        {"verbose",        no_argument, nullptr, 'v'},
        {"median",         no_argument, nullptr, 'm'},
        {"trader_info",    no_argument, nullptr, 'i'},
        {"time_traveler",  no_argument, nullptr, 't'},
        {nullptr,      0,               nullptr, '\0'},
    };  // longOptions[]
    // NOLINTEND

    while ((choice = getopt_long(argc, argv, "hvmit", static_cast<option *>(longOptions), &index)) != -1) {
        switch (choice) {
        case 'h':
            printHelp(*argv);
            exit(0);

        case 'v':
            options.verbose = true;
            break;

        case 'm':
            options.median = true;
            break;

        case 'i':
            options.trader_info = true;
            break;

        case 't':
            options.time_travelers = true;
            break;

        default:
            cerr << "Unknown option\n";
            exit(1);

        }
    }
}  // getOptions()

int main(int argc, char *argv[]){
  ios_base::sync_with_stdio(false);

  Options options;
  getOptions(argc, argv, options);

  Info i;

  return 0;
} // main()