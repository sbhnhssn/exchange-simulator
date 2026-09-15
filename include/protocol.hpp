#pragma once
#include "order_book.hpp"
#include <cstdint>
#include <string>
#include <vector>

enum class Command : uint8_t { NewOrder=1, Cancel=2, Snapshot=3 };
std::vector<uint8_t> encode_new_order(uint64_t id, Side side, OrderType type, int64_t price, uint64_t qty);
std::vector<uint8_t> encode_cancel(uint64_t id);
std::string snapshot_json(const OrderBook& book);
