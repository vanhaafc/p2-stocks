// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A2619B6

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "P2random.hpp"

using namespace std;

struct TraderStats { 
  int bought = 0;
  int sold = 0;
  int net_transfer = 0;
};

struct Trader { 
  int timestamp;
  bool buy;
  int trader_ID;
  int stock_ID;
  int price;
  int quantity;
  int order_number;
};

// Enum class for time traveler
enum class TimeStatus:char{
  NoTrades,
  CanBuy,
  Completed,
  Potential,
};

struct TimeTraveler
{
  TimeStatus state = TimeStatus::NoTrades;

  // Best buy (from a SELL order) and sell (to a BUY order) found so far
  int best_buy_price = 0;
  int best_buy_time = 0;
  int best_sell_price = 0;
  int best_sell_time = 0;

  // Lower SELL seen after a completed pair, might lead to a better profit
  int potential_buy_price = 0;
  int potential_buy_time = 0;
};

// Buyer comparator
struct compareBuyer{
  bool operator() (const Trader &a, const Trader &b) const {
    if(a.price == b.price) return a.order_number > b.order_number;
    return a.price < b.price;
  }
};


// Seller comparator
struct compareSeller{
  bool operator() (const Trader &a, const Trader &b) const {
    if(a.price == b.price) return a.order_number > b.order_number;
    return a.price > b.price;
  }
};

class Stock{
  private:
    // TODO: Only need price, quantity, trader_ID, and order_number, not any of the other Trader variables
    // how to change that??
    priority_queue<Trader, vector<Trader>, compareBuyer> buyers;
    priority_queue<Trader, vector<Trader>, compareSeller> sellers;

    // Put median tracker data structures here
    // We want the biggest of the small numbers
    priority_queue<int> small_half;
    // We want the smallest of the big numbers
    priority_queue<int, vector<int>, greater<>> big_half;

    // Time Travelers
    TimeTraveler tt;

  public:
    void add_to_median(int price){
      if(small_half.empty() || price <= small_half.top()){
        small_half.push(price);
      }
      else {
        big_half.push(price);
      }

      // Rebalance by moving the top of the bigger half over
      if(small_half.size() > big_half.size() + 1){
        big_half.push(small_half.top());
        small_half.pop();
      }
      else if(small_half.size() < big_half.size()){
        small_half.push(big_half.top());
        big_half.pop();
      }
    }

    void add_stock(Trader &t, bool time_travel){
      if(t.buy) buyers.push(t);
      else sellers.push(t);

      if(time_travel) update_time_traveler(t);
    }

    // Time traveler buys from SELL orders and sells to BUY orders
    void update_time_traveler(const Trader &t){
      switch(tt.state){
        case TimeStatus::NoTrades:
          if(!t.buy){
            tt.state = TimeStatus::CanBuy;
            tt.best_buy_price = t.price;
            tt.best_buy_time = t.timestamp;
          }
          break;

        case TimeStatus::CanBuy:
          if(!t.buy && t.price < tt.best_buy_price){
            tt.best_buy_price = t.price;
            tt.best_buy_time = t.timestamp;
          }
          else if(t.buy && t.price > tt.best_buy_price){
            tt.state = TimeStatus::Completed;
            tt.best_sell_price = t.price;
            tt.best_sell_time = t.timestamp;
          }
          break;

        case TimeStatus::Completed:
          if(t.buy && t.price > tt.best_sell_price){
            tt.best_sell_price = t.price;
            tt.best_sell_time = t.timestamp;
          }
          else if(!t.buy && t.price < tt.best_buy_price){
            tt.state = TimeStatus::Potential;
            tt.potential_buy_price = t.price;
            tt.potential_buy_time = t.timestamp;
          }
          break;

        case TimeStatus::Potential:
          if(!t.buy && t.price < tt.potential_buy_price){
            tt.potential_buy_price = t.price;
            tt.potential_buy_time = t.timestamp;
          }
          else if(t.buy && t.price - tt.potential_buy_price >
                  tt.best_sell_price - tt.best_buy_price){
            tt.state = TimeStatus::Completed;
            tt.best_buy_price = tt.potential_buy_price;
            tt.best_buy_time = tt.potential_buy_time;
            tt.best_sell_price = t.price;
            tt.best_sell_time = t.timestamp;
          }
          break;
      }
    }

    // Match buyers and sellers for this stock ID
    int match_traders(bool v, bool ti, bool m, vector<TraderStats> &stats){
      int count = 0;
      while(!buyers.empty() && !sellers.empty()){
        Trader b = buyers.top();
        Trader s = sellers.top();
        if(b.price < s.price) break;

        // Trade happens at the price of whichever order came first
        int price;
        if(b.order_number < s.order_number) price = b.price;
        else price = s.price;

        int amt_sold;
        if(b.quantity > s.quantity) amt_sold = s.quantity;
        else amt_sold = b.quantity;

        // Trader Info
        if(ti){
          size_t buyer_index = static_cast<size_t>(b.trader_ID);
          size_t seller_index = static_cast<size_t>(s.trader_ID);

          stats[buyer_index].bought += amt_sold;
          stats[seller_index].sold += amt_sold;
          stats[seller_index].net_transfer += price * amt_sold;
          stats[buyer_index].net_transfer -= price * amt_sold;
        }

        // Verbose
        if(v){
          cout << "Trader " << b.trader_ID << " purchased " << amt_sold <<
              " shares of Stock " << s.stock_ID << " from Trader " << s.trader_ID
              << " for $" << price << "/share\n";
        }

        if(m) add_to_median(price);

        // Pop the traders, push back whichever still has shares left
        buyers.pop();
        sellers.pop();
        b.quantity -= amt_sold;
        s.quantity -= amt_sold;
        if(b.quantity > 0) buyers.push(b);
        if(s.quantity > 0) sellers.push(s);

        count++;
      }

      return count;
    }

    // True once at least one trade has happened
    bool has_median(){
      return !small_half.empty();
    }

    int get_median(){
      if(small_half.size() != big_half.size()){
        return small_half.top();
      }
      else{
        return (small_half.top() + big_half.top()) / 2;
      }
    }

    // True if a time traveler could make a profit on this stock
    bool tt_has_profit(){
      return tt.state == TimeStatus::Completed || tt.state == TimeStatus::Potential;
    }

    int get_buy_time(){
      return tt.best_buy_time;
    }

    int get_buy_price(){
      return tt.best_buy_price;
    }

    int get_sell_time(){
      return tt.best_sell_time;
    }

    int get_sell_price(){
      return tt.best_sell_price;
    }
  
  };