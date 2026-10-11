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

  // 1 is the first order, 2 is the second, etc.
  int order_number;
};

// Buyer comparator
struct compareBuyer{
  bool operator() (const Trader a, const Trader b) const {
    if(a.price == b.price) return a.order_number > b.order_number;
    return a.price < b.price;
  }
};


// Seller comparator
struct compareSeller {
  bool operator() (const Trader a, const Trader b) const {
    if(a.price == b.price) return a.order_number > b.order_number;
    return a.price > b.price;
  }
};

class Stock{
  private:
    priority_queue<Trader, vector<Trader>, compareBuyer> buyers;
    priority_queue<Trader, vector<Trader>, compareSeller> sellers;

    // Put median tracker data structures here
    // We want the biggest of the small numbers
    priority_queue<int> small_half;
    // We want the smallest of the big numbers
    priority_queue<int, vector<int>, greater<>> big_half;

  public:
    void add_to_median(int price){
      if(small_half.empty() || price < big_half.top()){
        small_half.push(price);
      }
      else {
        big_half.push(price);
      }

      if(small_half.size() > big_half.size() + 1){
        big_half.push(price);
        small_half.pop();
      }
      else if(small_half.size() < big_half.size()){
        small_half.push(price);
        big_half.pop();
      }
    }

    void add_stock(Trader &t){
      if(t.buy) buyers.push(t);
      else sellers.push(t);

      add_to_median(t.price);
    }

    // Match buyers and sellers for this stock ID
    int match_traders(bool v, bool m, bool ti, vector<TraderStats> &stats){
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

          // Trader Info
          if(ti){
            int buyer_index = b.order_number;
            int seller_index = s.order_number;
            
            stats[buyer_index].bought += amt_sold;
            stats[seller_index].sold += amt_sold;
            stats[seller_index].net_transfer += s.price;
            stats[buyer_index].net_transfer -= s.price;
          }
          
          // Verbose
          if(v){
            cout << "Trader " << b.trader_ID << " purchased " << amt_sold << 
                " shares of Stock " << s.stock_ID << " from Trader " << s.trader_ID 
                << " for $" << s.price << "/share\n";
          }

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

    int get_median(){
      if(small_half.size() != big_half.size()){
        return small_half.top();
      }
      else{
        return small_half.top() + big_half.top() / 2;
      }
    }
  };