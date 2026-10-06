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
  // operator() for comparing traders? or is that a separate struct/functor
};

// TODO: Buyer comparator (organize by stock ID?)
struct compareBuyer{
  bool operator() (const Trader a, const Trader b) const {
    return a.price < b.price;
  }
};


// TODO: Seller comparator (organize by stock ID)
struct compareSeller {
  bool operator() (const Trader a, const Trader b) const {
    return a.price > b.price;
  }
};

class Stock{
  private:
    priority_queue<Trader, vector<Trader>, compareBuyer> buyers;
    priority_queue<Trader, vector<Trader>, compareSeller> sellers;

    // Put median tracker data structures here

  public:
    
};