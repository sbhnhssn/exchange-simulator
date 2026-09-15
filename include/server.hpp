#pragma once
#include "order_book.hpp"
#include "event_log.hpp"
#include <cstdint>
class ExchangeServer {
public:
 ExchangeServer(uint16_t port, OrderBook& book, EventLog& log):port_(port),book_(book),log_(log){}
 void run();
private:uint16_t port_;OrderBook& book_;EventLog& log_;
};
