// visitee list for stelaVisitor.
//   The base `stela` itself is dispatched through its own hand-written accept /
//   visit overloads and does not appear here: it is abstract, and the `def` block
//   it used to stand for is `defStela`. Every type listed here is concrete, so a
//   node of it can reach `visit()` at runtime. A type that does not override a
//   given onVisit / onLeave chains up to its `super` overload.
//
//   When a new leaf-typed stela subclass is added:
//     - register it with an X(T) line here;
//     - put BY(CLASS(T, ...), VISIT()) in its header;
//     - put BY(DEF_ME(T), DEF_VISIT()) in its impl.
X(defStela)
X(rootStela)
X(valStela)
X(arrStela)
X(strStela)
X(verStela)
X(nulStela)
