// Copyright (c) 2024 Chao Wang <hit9@icloud.com>.
// License: BSD. https://github.com/hit9/pdfsm.h

// A simple pushdown finite states machine library in C++.
//
// Requires: C++20
//
// verison 0.1.0

#ifndef HIT9_PDFSM_H
#define HIT9_PDFSM_H

#include <any>
#include <bitset>
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
};

//////////////////////
/// State
//////////////////////

// EnumClass constrains that the given type T must be an enum.
template <typename T>
concept EnumClass = std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>;

// Forward declaration of StateMachine.
template <EnumClass State>
class StateMachine;

template <EnumClass State>
class IStateBehavior {
 protected:
  StateMachine<State>* fsm = nullptr;
  virtual State StateValue(void) const = 0;
  virtual void BindFsm(StateMachine<State>* fsm) = 0;
  // Allows StateMachine to access method: StateValue and BindFsm.
  friend StateMachine<State>;

 public:
  IStateBehavior() = default;
  virtual ~IStateBehavior() = default;

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
  State StateValue(void) const final override { return EnumValue; }
  void BindFsm(StateMachine<State>* f) final override { this->fsm = f; }
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

template <EnumClass State>
class StateMachine {
  // N is the max value of this enum class, aka the size.
  static const int N = static_cast<int>(State::N);

 private:
  // static-array based stack, stores enum value's integers.
  int stack[N], top = 0;

  // (compressed) transition table, tt[from][to]
  std::bitset<N> tt[N];
  // behavior table, bt[integer of state] => raw pointer to the behavior instance.
  IStateBehavior<State>* bt[N];

 protected:
  // throws a runtime_error if the transition is invalid.
  inline void check(int from, int to) const {
    if (!tt[from][to])
      throw std::runtime_error("pdfsm: invalid jump from " + std::to_string(from) +
                               " to " + std::to_string(to));
  }
  inline int C(State state) const { return static_cast<int>(state); }

 public:
  StateMachine() = default;

  // Setup this state machine by a behaviors table and a transitions table.
  void Setup(const StateBehaviorTable<State>& behaviors,
             const TransitionTable<State>& transitions) {
    // Setup behaviors.
    for (auto& b : behaviors) {
      auto state = b->StateValue();
      bt[C(state)] = b.get();
      b->OnSetup();
      b->BindFsm(this);
    }

    // Setup transitions.
    for (auto& t : transitions)
      for (auto& to : t.targets) {
        tt[C(t.from)][C(to)] = 1;
      }
  }

  // Returns current active state.
  State Top(void) const { return static_cast<State>(stack[top]); }

  // Propagates ticking to current active state.
  void Update(const Context& ctx) {
    if (bt[stack[top]]->BeforeUpdate()) return;
    bt[stack[top]]->Update(ctx);
  }

  // Jump to a state.
  void Jump(const Context& ctx, State to) {
    int x = C(to);
    check(stack[top], x);
    bt[stack[top--]]->OnTerminate(ctx);
    stack[++top] = x;
    bt[x]->OnEnter(ctx);
  }

  // Pause current active state and push a new one.
  void Push(const Context& ctx, State to) {
    int x = C(to);
    check(stack[top], x);
    bt[stack[top]]->OnPause(ctx);
    stack[++top] = x;
    bt[x]->OnEnter(ctx);
  }

  // Pop current active state and resume the previous paused state.
  void Pop(const Context& ctx) {
    bt[stack[top--]]->OnTerminate(ctx);
    bt[stack[top]]->OnResume(ctx);
  }
};

}  // namespace pdfsm

#endif
