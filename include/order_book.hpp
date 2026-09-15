#pragma once
#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

enum class Side : uint8_t { Buy = 1, Sell = 2 };
enum class OrderType : uint8_t { Limit = 1, Market = 2 };

struct Order {
    uint64_t id{};
    Side side{};
    OrderType type{};
    int64_t price_ticks{}; // ignored for market orders
    uint64_t quantity{};
    uint64_t remaining{};
    uint64_t sequence{};
};

struct Trade {
    uint64_t buy_id{};
    uint64_t sell_id{};
    int64_t price_ticks{};
    uint64_t quantity{};
};

struct BookLevel {
    int64_t price_ticks{};
    uint64_t quantity{};
    size_t order_count{};
};

class OrderBook {
public:
    bool submit(Order order);
    bool cancel(uint64_t order_id);
    std::vector<Trade> last_trades() const { return trades_; }

    std::optional<int64_t> best_bid() const;
    std::optional<int64_t> best_ask() const;
    std::vector<BookLevel> bids(size_t depth = 10) const;
    std::vector<BookLevel> asks(size_t depth = 10) const;
    size_t resting_orders() const { return orders_.size(); }
    uint64_t next_sequence() const { return sequence_; }

private:
    using Queue = std::list<uint64_t>;
    using QueueIterator = Queue::iterator;
    std::map<int64_t, Queue, std::greater<>> bids_;
    std::map<int64_t, Queue> asks_;
    std::unordered_map<uint64_t, Order> orders_;
    // Direct iterator into the price-level queue: cancellation is O(1) after hash lookup.
    std::unordered_map<uint64_t, QueueIterator> positions_;
    uint64_t sequence_{1};
    std::vector<Trade> trades_;

    void match_buy(Order& incoming);
    void match_sell(Order& incoming);
    void rest(Order order);
    void erase_if_filled(uint64_t id);
};
