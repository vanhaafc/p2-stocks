// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2619B6

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Stock.hpp"

using namespace std;

// Enum class for time traveler
enum class TimeStatus:char{
  NoTrades,
  CanBuy,
  Completed,
  Potential
};

class Info{
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

    // TODO: add TraderStats to array when reading
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

      for(int i = 1; i <= num_stocks; i++){
        Stock s;
        stocks.push_back(s);
      }
    }

    // Matches the orders, call in read
    void match(){
      for(size_t i = 0; i < stocks.size(); i++){
        trade_count += stocks[i].match_traders(verbose, median, trader_info, stats);
      }
    }

    // Processing for PR
    void processOrders(P2random::GenStream &g) {
      // Read orders from inputStream, NOT cin
      Trader t;
      char c;
      string buy_sell;
      int previous_timestamp = 0;
      string new_line;
      while (g >> t.timestamp >> buy_sell >> c >> t.trader_ID 
              >> c >> t.stock_ID >> c >> t.price >> c >> t.quantity >> new_line) {
        // Check for decreasing timestamp
        if(t.timestamp < previous_timestamp){
          cerr << "Decreasing timestamp";
          exit(1);
        }

        // Median - print if timestamp changes
        if(t.timestamp > previous_timestamp && median){
          for(size_t i = 0; i < stocks.size(); i++){
            cout << "Median match price of Stock " << i << " at time " 
            << previous_timestamp << " is $" << stocks[i].get_median() << "\n";
          }
        }

        previous_timestamp = t.timestamp;

        if(buy_sell == "BUY") t.buy = true;
        else t.buy = false;

        // Check for invalid trader ID
        if(t.trader_ID < 0 || t.trader_ID >= num_traders){
          cerr << "Invalid trader ID";
          exit(1);
        }

        // Check for invalid stock ID
        if(t.stock_ID < 0 || t.stock_ID >= num_stocks){
          cerr << "Invalid stock ID";
          exit(1);
        }

        // Check for price <= 0
        if(t.price <= 0){
          cerr << "Invalid price (<= 0)";
          exit(1);
        }

        // Check for quantity <= 0
        if(t.quantity <= 0){
          cerr << "Invalid quantity (<= 0)";
          exit(1);
        }

        t.order_number = order_priority;
        order_priority++;

        if(trader_info){
          TraderStats ts;
          ts.bought = 0;
          ts.sold = 0;
          ts.net_transfer = 0;
          stats.push_back(ts);
        }

        // Add Trader to that stockID
        stocks[t.stock_ID].add_stock(t);

        // Match the stocks
        match();
      } // while
    } // processOrders()

    // Processing for TL
    void processOrders(istream &inputStream) {
      // Read orders from inputStream, NOT cin
      Trader t;
      char c;
      string buy_sell;
      int previous_timestamp = 0;
      string new_line;
      while (inputStream >> t.timestamp >> buy_sell >> c >> t.trader_ID 
              >> c >> t.stock_ID >> c >> t.price >> c >> t.quantity >> new_line) {
        // Check for decreasing timestamp
        if(t.timestamp < previous_timestamp){
          cerr << "Decreasing timestamp";
          exit(1);
        }
        previous_timestamp = t.timestamp;

        if(buy_sell == "BUY") t.buy = true;
        else t.buy = false;

        // Check for invalid trader ID
        if(t.trader_ID < 0 || t.trader_ID >= num_traders){
          cerr << "Invalid trader ID";
          exit(1);
        }

        // Check for invalid stock ID
        if(t.stock_ID < 0 || t.stock_ID >= num_stocks){
          cerr << "Invalid stock ID";
          exit(1);
        }

        // Check for price <= 0
        if(t.price <= 0){
          cerr << "Invalid price (<= 0)";
          exit(1);
        }

        // Check for quantity <= 0
        if(t.quantity <= 0){
          cerr << "Invalid quantity (<= 0)";
          exit(1);
        }

        t.order_number = order_priority;
        order_priority++;

        if(trader_info){
          TraderStats ts;
          ts.bought = 0;
          ts.sold = 0;
          ts.net_transfer = 0;
          stats.push_back(ts);
        }

        // Add Trader to that stockID
        stocks[t.stock_ID].add_stock(t);

        // Match the stocks
        match();
      } // while
    } // processOrders()

    // Read in the input file
    void read(){
      cout << "Processing orders...\n";

      string junk;

      getline(cin, comment);

      cin >> junk;
      cin >> input_mode;
      if(input_mode == "PR") pr = true;
      cin >> junk;

      cin >> junk;
      cin >> num_traders;
      cin >> junk;

      cin >> junk;
      cin >> num_stocks;
      cin >> junk;

      P2random::GenStream gen;
      
      if(pr){
        int seed;
        int num_orders;
        int rate;

        string junk;
        cin >> junk;
        cin >> seed;
        cin >> num_orders;
        cin >> rate;

        gen.init(seed, num_traders, num_stocks, num_orders, rate);
        processOrders(gen);
      }
      else{
        processOrders(cin);
      }

      cout << "---End of Day---\n" << "Trades Completed: " << trade_count << "\n";

      if(trader_info){
          cout << "---Trader Info---\n"; 
          for (int i = 0; i < num_traders; ++i) { 
            cout << "Trader " << i << "bought" << stats[i].bought 
            << " and sold " << stats[i].sold << " for a net transfer of " 
            << stats[i].net_transfer << "\n";
          }
      }
    } // read()
  };