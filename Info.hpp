// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2619B6

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Stock.hpp"

using namespace std;


class Info{
  private:
    bool pr = false;

    string comment;
    string input_mode;
    int num_traders;
    int num_stocks;

    vector<Stock> stocks;

  public:

    // Default Constructor
    Info(){
      comment = "";
      input_mode = "";
      num_traders = 0;
      num_stocks = 0;
    }

    // TODO: Matches the orders?, call in read?
    // TODO: all the options (verbose, median, trader_info, time traveler)
    void match(){

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

        // TODO: fix
        //if(t.buy) buyers.push(t);
        //else sellers.push(t);
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

        // TODO: fix
        //if(t.buy) buyers.push(t);
        //else sellers.push(t);
        
      } // while
    } // processOrders()

    // Read in the input file
    void read(){
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
    } // read()
};