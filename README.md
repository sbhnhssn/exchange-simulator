# High-Performance Exchange Engine

A C++20 educational exchange simulator designed as a systems-programming portfolio project.

## Features

- Price-time priority limit order book
- Limit and market orders
- Partial fills and cancellations
- Fixed-point integer prices (no floating-point money)
- O(1)-average order lookup by ID
- Append-only binary event log
- Deterministic replay/recovery
- Compact length-prefixed TCP protocol
- Multi-client threaded server
- Market-data snapshots
- Unit and property-style invariant tests
- Benchmark mode

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Run:

```bash
./build/exchange --demo
./build/exchange --benchmark 1000000
./build/exchange --server 9000
./build/exchange --replay exchange.log
```

## Design

Prices are integer ticks. An order is identified by a unique 64-bit ID. Each price level owns a FIFO queue, preserving price-time priority.

The engine separates:
1. Matching state
2. Persistence
3. Wire protocol
4. Networking

This makes deterministic replay possible: the event log contains the commands that created state, and recovery applies them in exactly the same order.

## Complexity

Let P be the number of price levels and N the number of resting orders.

- Order lookup: O(1) average
- Cancel: O(1) average after lookup, plus queue unlinking
- Matching: proportional to the number of executions/levels crossed
- Snapshot: O(P + N)

## Correctness invariants

The tests verify that:
- best bid is strictly below best ask after matching completes
- cancelled orders cannot trade
- fills never exceed available quantity
- replay reproduces observable book state
- market orders never become resting orders
- order IDs are unique

## Performance work

The benchmark is intentionally simple. For a serious report, run it on your own machine and record:
- throughput
- median latency
- p99 latency
- allocations
- CPU usage
- memory usage

Do not copy benchmark numbers from another machine into a CV.

## Roadmap

Possible further research:
- lock-free market-data publication
- single-writer/multi-reader architecture
- custom slab allocator
- binary multicast market-data feed
- sequence numbers and gap detection
- WAL checksums
- snapshot + log recovery
- property-based fuzzing
- flamegraph/perf analysis
- NUMA-aware experiments

## Suggested portfolio presentation

Show a short architecture diagram, correctness invariants, benchmark methodology, and a profiling screenshot from your own machine. The strongest story is not a raw throughput number; it is a measured before/after optimization with an explanation of the bottleneck.

Recommended next experiments:
- compare `std::map` with a flat/array-based price ladder for bounded tick ranges
- replace per-order heap allocations with a slab/free-list
- benchmark p50/p95/p99 latency
- add event sequence numbers and CRC32
- add snapshot checkpoints for fast recovery
- fuzz random order/cancel sequences against a simple reference model
