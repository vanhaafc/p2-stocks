// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2619B6

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Stock.hpp"

using namespace std;

class Info
{
private:
  bool pr = false;

  string comment;
  string input_mode;
  int num_traders;
  int num_stocks;

  vector<Stock> stocks;

  bool verbose;
  bool median;
  bool trader_info;
  bool time_travelers;

  int trade_count;
  int order_priority;

  // Trader Info
  vector<TraderStats> stats;

public:
  // Default Constructor
  Info(bool v, bool m, bool ti, bool tt){
    verbose = v;
    median = m;
    trader_info = ti;

    time_travelers = tt;

    comment = "";
    input_mode = "";
    num_traders = 0;
    num_stocks = 0;
    trade_count = 0;
    order_priority = 0;
  }

  // Matches the orders, call in read
  void match(){
    for (size_t i = 0; i < stocks.size(); i++){
      trade_count += stocks[i].match_traders(verbose, trader_info, median, stats);
    }
  }

  // Print the median for every stock that has had a trade
  void print_medians(int timestamp){
    for (size_t i = 0; i < stocks.size(); i++){
      if (stocks[i].has_median()){
        cout << "Median match price of Stock " << i << " at time "
             << timestamp << " is $" << stocks[i].get_median() << "\n";
      }
    }
  }

  void process(Trader &t, string b_s, int &previous_timestamp){
    // Check for decreasing timestamp
    if (t.timestamp < previous_timestamp){
      cerr << "Error: Decreasing timestamp";
      exit(1);
    }

    // Median Output
    if (t.timestamp > previous_timestamp && median){
      print_medians(previous_timestamp);
    }

    previous_timestamp = t.timestamp;

    if (b_s == "BUY") t.buy = true;
    else t.buy = false;

    // Check for invalid trader ID
    if (t.trader_ID < 0 || t.trader_ID >= num_traders){
      cerr << "Error: Invalid trader ID";
      exit(1);
    }

    // Check for invalid stock ID
    if (t.stock_ID < 0 || t.stock_ID >= num_stocks){
      cerr << "Error: Invalid stock ID";
      exit(1);
    }

    // Check for price <= 0
    if (t.price <= 0){
      cerr << "Error: Invalid price (<= 0)";
      exit(1);
    }

    // Check for quantity <= 0
    if (t.quantity <= 0){
      cerr << "Error: Invalid quantity (<= 0)";
      exit(1);
    }

    t.order_number = order_priority;
    order_priority++;

    // Add Trader to that stockID
    stocks[static_cast<size_t>(t.stock_ID)].add_stock(t, time_travelers);
  }

  // Processing for PR
  void processOrders(P2random::GenStream &g){
    // Read orders from inputStream, NOT cin
    Trader t;
    char c;
    string buy_sell;
    int previous_timestamp = 0;
    while (g >> t.timestamp >> buy_sell >> c >> t.trader_ID >> c >> t.stock_ID >> c >> t.price >> c >> t.quantity){
      // Process order
      process(t, buy_sell, previous_timestamp);

      // Match the stocks
      match();
    } // while

    if (median) print_medians(previous_timestamp);
  } // processOrders()

  // Processing for TL
  void processOrders(istream &inputStream){
    // Read orders from inputStream, NOT cin
    Trader t;
    char c;
    string buy_sell;
    int previous_timestamp = 0;
    while (inputStream >> t.timestamp >> buy_sell >> c >> t.trader_ID >> c >> t.stock_ID >> c >> t.price >> c >> t.quantity){
      // Process order
      process(t, buy_sell, previous_timestamp);

      // Match stocks
      match();
    } // while

    if (median) print_medians(previous_timestamp);
  } // processOrders()

  // Read in the input file
  void read(){
    cout << "Processing orders...\n";

    string junk;

    getline(cin, comment);

    cin >> junk;
    cin >> input_mode;
    if (input_mode == "PR") pr = true;

    cin >> junk;
    cin >> num_traders;

    cin >> junk;
    cin >> num_stocks;
    stocks.resize(static_cast<size_t>(num_stocks));
    if (trader_info) stats.resize(static_cast<size_t>(num_traders));
    for (size_t i = 0; i < static_cast<size_t>(num_stocks); i++){
      Stock s;
      stocks[i] = s;
    }

    P2random::GenStream gen;

    if (pr){
      uint32_t seed;
      uint32_t num_orders;
      uint32_t rate;

      string junk;
      cin >> junk;
      cin >> seed;
      cin >> junk;
      cin >> num_orders;
      cin >> junk;
      cin >> rate;

      gen.init(seed, static_cast<uint32_t>(num_traders), static_cast<uint32_t>(num_stocks), num_orders, rate);
      processOrders(gen);
    }
    else{
      processOrders(cin);
    }

    cout << "---End of Day---\n"
         << "Trades Completed: " << trade_count << "\n";

    // Trader Info Ooutput
    if (trader_info){
      cout << "---Trader Info---\n";
      for (size_t i = 0; i < static_cast<size_t>(num_traders); ++i)
      {
        cout << "Trader " << i << " bought " << stats[i].bought
             << " and sold " << stats[i].sold << " for a net transfer of $"
             << stats[i].net_transfer << "\n";
      }
    }

    // Time Travelers Output
    if (time_travelers){
      cout << "---Time Travelers---\n";
      for (size_t i = 0; i < stocks.size(); i++){
        Stock &s = stocks[i];
        if (s.tt_has_profit()){
          cout << "A time traveler would buy Stock " << i << " at time " << s.get_buy_time()
               << " for $" << s.get_buy_price() << " and sell it at time " << s.get_sell_time()
               << " for $" << s.get_sell_price() << "\n";
        }
        else{
          cout << "A time traveler could not make a profit on Stock " << i << "\n";
        }
      }
    }
  } // read()
};