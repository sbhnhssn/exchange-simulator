#include "order_book.hpp"
#include "event_log.hpp"
#include <cassert>
#include <cstdio>
int main(){
 {OrderBook b;
  assert(b.submit({1,Side::Buy,OrderType::Limit,100,10,0,0}));
  assert(b.submit({2,Side::Sell,OrderType::Limit,101,5,0,0}));
  assert(b.submit({3,Side::Sell,OrderType::Limit,100,7,0,0}));
  auto t=b.last_trades(); assert(t.size()==1 && t[0].quantity==7 && t[0].price_ticks==100);
  assert(b.resting_orders()==2);
  assert(b.cancel(1)); assert(!b.cancel(1));
 }
 {OrderBook b;
  b.submit({1,Side::Buy,OrderType::Limit,100,10,0,0});
  b.submit({2,Side::Buy,OrderType::Limit,100,10,0,0});
  b.submit({3,Side::Sell,OrderType::Market,0,15,0,0});
  auto t=b.last_trades(); assert(t.size()==2); assert(t[0].buy_id==1 && t[1].buy_id==2);
 }
 std::remove("test.log");
 {EventLog l("test.log");l.append_submit({1,Side::Buy,OrderType::Limit,100,10,0,0});l.append_cancel(1);auto e=l.replay();assert(e.size()==2);assert(e[0].order.id==1&&e[1].cancel_id==1);}
 std::remove("test.log");
 return 0;
}
