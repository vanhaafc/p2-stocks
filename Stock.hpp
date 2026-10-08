#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "P2random.hpp"

using namespace std;

struct Trader { 
  int timestamp;
  bool buy;
  int trader_ID;
  int stock_ID;
  int price;
  int quantity;

  // For trader info output
  int net_transfer;
  int num_bought;
  int num_sold;
};

// Buyer comparator
struct compareBuyer{
  bool operator() (const Trader a, const Trader b) const {
    // TODO: switch to greater than??
    return a.price < b.price;
  }
};


// Seller comparator
struct compareSeller {
  bool operator() (const Trader a, const Trader b) const {
    // TODO: switch to less than??
    return a.price > b.price;
  }
};

class Stock{
  private:
    priority_queue<Trader, vector<Trader>, compareBuyer> buyers;
    priority_queue<Trader, vector<Trader>, compareSeller> sellers;

    // Put median tracker data structures here


  public:
    void add_stock(Trader &t){
      if(t.buy) buyers.push(t);
      else sellers.push(t);
    }

    // TODO: Match buyers and sellers for this stock ID
    // TODO: print options??? (verbose, median, trader_info, time_traveler)
    int match_traders(bool v, bool m, bool ti){
      int count = 0;
      bool matching = true;
      while(matching){
        Trader b = buyers.top();
        Trader s = sellers.top();
        if(!buyers.empty() && !sellers.empty() && b.price >= s.price){
          // Execute order
          int amt_sold;
          if(b.quantity > s.quantity) amt_sold = s.quantity;
          else amt_sold = b.quantity;

          // Verbose
          if(v){
            cout << "Trader " << b.trader_ID << " purchased " << amt_sold << 
                " shares of Stock " << s.stock_ID << " from Trader " << s.trader_ID 
                << " for $" << s.price << "/share\n";
          }
          
          
          // TODO: Options

          // Pop the traders
          buyers.pop();
          sellers.pop();

          count++;
        }
        else{
          matching = false;
        }
      }
      
      return count;
    }
};