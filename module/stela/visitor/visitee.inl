// visitee list for stelaVisitor.
//   Abstract types do not appear here: `stela` and `valStela` are dispatched through
//   hand-written overloads in stelaVisitor instead, since no node of theirs exists at
//   runtime to reach `visit()`. Every type listed here is concrete. A type that does
//   not override a given onVisit / onLeave chains up to its `super` overload.
//
//   When a new leaf-typed stela subclass is added:
//     - register it with an X(T) line here;
//     - put BY(CLASS(T, ...), VISIT()) in its header;
//     - put BY(DEF_ME(T), DEF_VISIT()) in its impl.
X(defStela)
X(rootStela)
X(arrStela)
X(numStela)
X(boolStela)
X(strStela)
X(verStela)
X(nulStela)
