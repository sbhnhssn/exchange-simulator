#include "protocol.hpp"
#include <sstream>
template<class T> static void put(std::vector<uint8_t>& b,T x){for(size_t i=0;i<sizeof(T);++i)b.push_back(uint8_t((uint64_t(x)>>(8*i))&255));}
std::vector<uint8_t> encode_new_order(uint64_t id,Side s,OrderType t,int64_t p,uint64_t q){
 std::vector<uint8_t>b{uint8_t(Command::NewOrder)};put(b,id);b.push_back(uint8_t(s));b.push_back(uint8_t(t));put(b,p);put(b,q);return b;
}
std::vector<uint8_t> encode_cancel(uint64_t id){std::vector<uint8_t>b{uint8_t(Command::Cancel)};put(b,id);return b;}
std::string snapshot_json(const OrderBook& book){
 std::ostringstream o;o<<"{\"bids\":[";
 bool first=true;for(auto x:book.bids()){if(!first)o<<",";first=false;o<<"["<<x.price_ticks<<","<<x.quantity<<"]";}
 o<<"],\"asks\":[";first=true;for(auto x:book.asks()){if(!first)o<<",";first=false;o<<"["<<x.price_ticks<<","<<x.quantity<<"]";}o<<"]}";
 return o.str();
}
