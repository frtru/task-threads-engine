# task-threads-engine
Simple implementation of a task engine. 

This is merely an exploration attempt at the various problematics that one faces when implementing a task/worker-threads design.

* Object pools utilities for faster instantiation using heap and avoiding fragmentation.
* Generic PoolableObject class
* Delegation of _tasks_ towards a thread pool
* Scheduler deducing which task to send next (different policies like priorities could be used, least busy workers, affinity, etc.)
* Macro tasks or tasks with children
