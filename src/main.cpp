#include "order_book.hpp"
#include "event_log.hpp"
#include "server.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <string>
int main(int argc,char**argv){
 try{
  if(argc>=2 && std::string(argv[1])=="--benchmark"){
   size_t n=argc>=3?std::stoull(argv[2]):1000000; OrderBook b;
   auto t=std::chrono::steady_clock::now();
   for(size_t i=0;i<n;++i){Order o{uint64_t(i+1),i%2?Side::Buy:Side::Sell,OrderType::Limit,10000+int64_t(i%100),100,0,0};b.submit(o);}
   auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t).count();
   std::cout<<"orders="<<n<<" elapsed_us="<<us<<" throughput="<<(double(n)*1e6/double(us))<<" orders/s\n";return 0;
  }
  EventLog log("exchange.log"); OrderBook b;
  if(argc>=2&&std::string(argv[1])=="--replay"){for(auto&e:log.replay())if(e.kind==Event::Kind::Submit)b.submit(e.order);else b.cancel(e.cancel_id);std::cout<<"replayed "<<log.replay().size()<<" events\n";return 0;}
  if(argc>=2&&std::string(argv[1])=="--server"){uint16_t p=argc>=3?uint16_t(std::stoi(argv[2])):9000;ExchangeServer s(p,b,log);s.run();return 0;}
  b.submit({1,Side::Buy,OrderType::Limit,10000,100,0,0});
  b.submit({2,Side::Sell,OrderType::Limit,9990,40,0,0});
  for(auto&t:b.last_trades())std::cout<<"trade "<<t.buy_id<<" "<<t.sell_id<<" "<<t.price_ticks<<" "<<t.quantity<<"\n";
 }catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}
}
