#include "event_log.hpp"
#include <fstream>
#include <stdexcept>

namespace {
template<class T> void put(std::vector<uint8_t>& b, T x) {
    for (size_t i=0;i<sizeof(T);++i) b.push_back(static_cast<uint8_t>((static_cast<uint64_t>(x) >> (8*i)) & 0xff));
}
template<class T> T get(const std::vector<uint8_t>& b, size_t& p) {
    uint64_t x=0;
    for(size_t i=0;i<sizeof(T);++i) x |= uint64_t(b[p++]) << (8*i);
    return static_cast<T>(x);
}
}
EventLog::EventLog(std::string path): path_(std::move(path)) {}
void EventLog::append(const std::vector<uint8_t>& bytes) {
    std::ofstream f(path_, std::ios::binary|std::ios::app);
    if(!f) throw std::runtime_error("cannot open event log");
    uint32_t n = static_cast<uint32_t>(bytes.size());
    f.write(reinterpret_cast<const char*>(&n), sizeof(n));
    f.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    f.flush();
}
void EventLog::append_submit(const Order& o) {
    std::vector<uint8_t> b{static_cast<uint8_t>(Event::Kind::Submit)};
    put(b,o.id); b.push_back(static_cast<uint8_t>(o.side)); b.push_back(static_cast<uint8_t>(o.type));
    put(b,o.price_ticks); put(b,o.quantity);
    append(b);
}
void EventLog::append_cancel(uint64_t id) {
    std::vector<uint8_t> b{static_cast<uint8_t>(Event::Kind::Cancel)}; put(b,id); append(b);
}
std::vector<Event> EventLog::replay() const {
    std::ifstream f(path_, std::ios::binary);
    std::vector<Event> out;
    if(!f) return out;
    while(true) {
        uint32_t n=0;
        if(!f.read(reinterpret_cast<char*>(&n),sizeof(n))) break;
        std::vector<uint8_t> b(n);
        if(!f.read(reinterpret_cast<char*>(b.data()),n)) break; // ignore torn final record
        size_t p=1;
        auto kind=static_cast<Event::Kind>(b[0]);
        if(kind==Event::Kind::Submit) {
            Order o; o.id=get<uint64_t>(b,p); o.side=static_cast<Side>(b[p++]); o.type=static_cast<OrderType>(b[p++]);
            o.price_ticks=get<int64_t>(b,p); o.quantity=get<uint64_t>(b,p); out.push_back({kind,o,0});
        } else if(kind==Event::Kind::Cancel) out.push_back({kind,{},get<uint64_t>(b,p)});
    }
    return out;
}
