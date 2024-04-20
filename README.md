pdfsm.h
=======

![](https://github.com/hit9/pdfsm.h/actions/workflows/tests.yml/badge.svg)
![](https://img.shields.io/badge/license-BSD3-brightgreen)

A simple pushdown finite states machine library that separates data and behaviors as much as possible.

### In brief

1. States are just enum values.
2. Implement behaviors by inheriting from `pdfsm::B`
3. A state behavior class shouldn't contain any internal data.
4. A StateMachine is just a struct, holding active states.
5. `StateMachineHandler` is for handling each state machine's behavior.

### Code Example

1. Defines state enums, there the trailing `N` is necessary:

   ```cpp
   enum class RobotState { Idle, Moving, Dancing, N };
   ```

2. Makes a transition table, `jump`s or `push`s that violate this table will throw an exception

   ```cpp
   // for each item: { from, {to list} }
   pdfsm::TransitionTable<RobotState> transitions{
       {RobotState::Idle, {RobotState::Moving, RobotState::Dancing}},
       {RobotState::Moving, {RobotState::Idle, RobotState::Dancing}},
       {RobotState::Dancing, {RobotState::Idle}},
   };
   ```

3. Defines a state behavior class for each state:

   ```cpp
   class RobotMovingBehavior : public pdfsm::B<RobotState::Moving> {
    public:
     void OnSetup() override {}
     void Update(const pdfsm::Context& ctx) override {}
     void OnEnter(const pdfsm::Context& ctx) override {}
     void OnTerminate(const pdfsm::Context& ctx) override { }
     void OnPause(const pdfsm::Context& ctx) override {}
     void OnResume(const pdfsm::Context& ctx) override {}
   };
   ```

   And a behavior table:

   ```cpp
   pdfsm::BTable<RobotState> behaviors{
       std::make_unique<RobotIdleBehavior>(),
       std::make_unique<RobotMovingBehavior>(),
       std::make_unique<RobotDancingBehavior>(),
   };
   ```

4. Creates a state machine `fsm`.

   A `fsm` is just a struct holding active states in a static-array based stack.
   It can be used as an attribute (or field) of, i.e. a game entity.

   ```cpp
   pdfsm::StateMachine<RobotState> fsm;
   ```

5. Makes a context for ticking / update, for propagating to the active state's hook methods:

   ```cpp
   pdfsm::Context ctx;
   ```

6. Creates a handler to manipulate a fsm's state transitions:

   ```cpp
   pdfsm::StateMachineHandler<RobotState> h(behaviors, transitions);
   ```

   Before handling a fsm struct, binds it at first:

   ```cpp
   h.SetHandlingFsm(fsm, ctx);
   ```

   And then, after handling, clears the binding:

   ```cpp
   h.ClearHandlingFsm();
   ```

7. We can access the handler's pointer inside a state behavior class:

   ```cpp
   auto& handler = GetHandler(); // returns a pointer to the handler.
   ```

8. Operations / APIs of the handler:

   ```cpp
   // Jump to a state.
   handler->Jump(ctx, RobotState::Moving);

   // Push a state and pause current active state.
   handler->Push(ctx, RobotState::Dancing);

   // Pop current active state and resume previous paused state.
   handler->Pop(ctx);

   // Gets current active state.
   RobotState state = handler->Top();

   // Update (ticking).
   handler->Update(ctx);
   ```

To work with signals, for example, with my tiny signal/event library [blinker.h](https://github.com/hit9/blinker.h),
checkout [tests](tests/states.h).

### License

BSD.
