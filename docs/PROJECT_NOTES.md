# Engineering notes

## Why integer prices?
Prices are stored as integer ticks rather than floating point, avoiding monetary rounding errors.

## Persistence
Accepted commands are appended to an event log. Recovery replays those commands deterministically. A production implementation should add checksums, sequence numbers, snapshots, and explicit durable-fsync semantics.

## Concurrency
The server uses a mutex around mutations as a clear baseline. A stronger architecture would use a single-writer exchange core and lock-free/read-optimized market-data publication.

## Production caveat
This is a portfolio simulator, not a production exchange. It does not implement authentication, encryption, FIX, certification, regulatory controls, or production-grade durability.
