// Copyright (c) 2024 Chao Wang <hit9@icloud.com>.
// License: BSD. https://github.com/hit9/pdfsm.h

// A simple pushdown finite states machine library that
// separates data and behaviors as much as possible.
//
// 1. States are just enum values.
// 2. Implement behaviors by inheriting from pdfsm::B
// 3. A behavior class shouldn't contain any internal data.
// 4. A StateMachine is just a struct, holding active states.
// 5. StateMachineHandler is for handling each state machine's behavior.
//
// Requires: C++20
//
// verison 0.1.0

#ifndef HIT9_PDFSM_H
#define HIT9_PDFSM_H

#include <any>
#include <bitset>
#include <cassert>
#include <chrono>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace pdfsm {

//////////////////////
/// Update Context
//////////////////////

// Ticking context.
struct Context {
  // ticking seq number.
  unsigned long long seq;
  // delta time since last tick.
  std::chrono::nanoseconds delta;
  // user data.
  std::any data;

  Context() = default;
  Context(std::any data) : data(data) {}
};

//////////////////////
/// State
//////////////////////

// EnumClass constrains that the given type T must be an integeral enum.
template <typename T>
concept EnumClass = std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>;

template <EnumClass State>
class StateMachineHandler;  // Forward declaration of StateMachineHandler.

template <EnumClass State>
class IStateBehaviorBase {
  using Handler = StateMachineHandler<State>;

 private:
  Handler* handler = nullptr;
  void bindHandler(Handler* h) { handler = h; }
  friend Handler;

 protected:
  Handler& GetHandler() { return *handler; }
};

// StateBehavior interface.
template <EnumClass State>
class IStateBehavior : public IStateBehaviorBase<State> {
 public:
  IStateBehavior() = default;
  virtual ~IStateBehavior() = default;
  virtual State StateValue(void) const = 0;

  ///////////////////////////////
  /// Overridable Hook Methods
  //////////////////////////////

  virtual void OnSetup() {}
  virtual void OnEnter(const Context& ctx) {}
  virtual void OnTerminate(const Context& ctx) {}
  virtual void OnPause(const Context& ctx) {}
  virtual void OnResume(const Context& ctx) {}
  virtual bool BeforeUpdate(const Context& ctx) { return false; }
  virtual void Update(const Context& ctx) {}
};

// internal helper class.
template <EnumClass State, State EnumValue>
struct _s {};

template <auto EnumValue>
class StateBehavior : public _s<decltype(EnumValue), EnumValue>,
                      public IStateBehavior<decltype(EnumValue)> {
 protected:
  using State = decltype(EnumValue);

 public:
  State StateValue(void) const final override { return EnumValue; }
};

template <auto EnumValue>
using B = StateBehavior<EnumValue>;  // alias

template <EnumClass State>
using StateBehaviorTable = std::initializer_list<std::unique_ptr<IStateBehavior<State>>>;

template <EnumClass State>
using BTable = StateBehaviorTable<State>;  // alias

//////////////////////
/// Transition
//////////////////////

template <EnumClass State>
struct Transition {
  State from;
  std::initializer_list<State> targets;
};

template <EnumClass State>
using TransitionTable = std::initializer_list<Transition<State>>;

//////////////////////
/// StateMachine
//////////////////////

// StateMachine is just plain struct storing active states.
template <EnumClass State>
struct StateMachine {
  // N is the max value of this enum class, aka the size.
  static const int N = static_cast<int>(State::N);
  // static-array based stack, stores enum value's integers.
  // The initial state is state 0.
  // top=-1 meaning this state machine still not started.
  int stack[N], top = -1;
};

/////////////////////////
/// StateMachineHandler
/////////////////////////

template <EnumClass State>
class StateMachineHandler {
 private:
  // N is the max value of this enum class, aka the size.
  static const int N = static_cast<int>(State::N);
  // (Compressed) transition table, tt[from][to]
  std::bitset<N> tt[N];
  // Behavior pointers array.
  // bt[state enum integer] => raw pointer to the behavior instance.
  IStateBehavior<State>* bt[N];
  // Currently processing fsm.
  StateMachine<State>* m = nullptr;

 protected:
  // throws a runtime_error if the transition is invalid.
  inline void check(int from, int to) const {
    if (!tt[from][to])
      throw std::runtime_error("pdfsm: invalid jump from " + std::to_string(from) +
                               " to " + std::to_string(to));
  }
  inline int C(State state) const { return static_cast<int>(state); }

  // Setup this state machine by a behaviors table and a transitions table.
  void setup(const StateBehaviorTable<State>& behaviors,
             const TransitionTable<State>& transitions) {
    // Setup behaviors.
    for (auto& b : behaviors) {
      auto state = b->StateValue();
      bt[C(state)] = b.get();
      b->bindHandler(this);
      b->OnSetup();
    }

    // Setup transitions.
    for (auto& t : transitions)
      for (auto& to : t.targets) {
        tt[C(t.from)][C(to)] = 1;
      }
  }

 public:
  StateMachineHandler(const auto& behaviors, const auto& transitions) {
    setup(behaviors, transitions);
  }

  // Sets current handling fsm.
  void SetHandlingFsm(StateMachine<State>& fsm, const Context& ctx) {
    m = &fsm;
    if (m->top == -1) Jump(ctx, static_cast<State>(0));
  }

  // Clears current handling fsm.
  void ClearHandlingFsm(void) { m = nullptr; }

  // Returns current active state.
  State Top(void) const {
    assert(m != nullptr);
    assert(m->top >= 0);
    return static_cast<State>(m->stack[m->top]);
  }

  // Propagates ticking to current active state.
  void Update(const Context& ctx) {
    assert(m != nullptr);
    if (bt[m->stack[m->top]]->BeforeUpdate(ctx)) return;
    bt[m->stack[m->top]]->Update(ctx);
  }

  // Jump to a state.
  void Jump(const Context& ctx, const State& to) {
    assert(m != nullptr);
    int x = C(to);
    if (m->top != -1) {
      check(m->stack[m->top], x);
      bt[m->stack[m->top--]]->OnTerminate(ctx);
    }
    m->stack[++m->top] = x;
    bt[x]->OnEnter(ctx);
  }

  // Pause current active state and push a new one.
  void Push(const Context& ctx, const State& to) {
    assert(m != nullptr);
    int x = C(to);
    if (m->top != -1) {
      check(m->stack[m->top], x);
      bt[m->stack[m->top]]->OnPause(ctx);
    }
    m->stack[++m->top] = x;
    bt[x]->OnEnter(ctx);
  }

  // Pop current active state and resume the previous paused state.
  void Pop(const Context& ctx) {
    assert(m != nullptr);
    assert(m->top >= 0 );
    bt[m->stack[m->top--]]->OnTerminate(ctx);
    bt[m->stack[m->top]]->OnResume(ctx);
  }
};
}  // namespace pdfsm

#endif
