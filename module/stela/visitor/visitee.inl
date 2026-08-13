// visitee list for stelaVisitor.
//   The base `stela` itself is dispatched through its own hand-written accept /
//   visit overloads and does not appear here. Intermediate ADTs (e.g. `valStela`)
//   are listed too so their onVisit / onLeave overloads exist as chain-up targets
//   from concrete subclasses; their `visit()` overload is never invoked at
//   runtime because no instance is constructed.
//
//   When a new leaf-typed stela subclass is added:
//     - register it with an X(T) line here;
//     - put BY(CLASS(T, ...), VISIT()) in its header;
//     - put BY(DEF_ME(T), DEF_VISIT()) in its impl.
X(valStela)
X(strStela)
X(verStela)
X(nulStela)
