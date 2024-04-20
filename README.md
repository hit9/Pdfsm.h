pdfsm.h
=======

![](https://github.com/hit9/pdfsm.h/actions/workflows/tests.yml/badge.svg)
![](https://img.shields.io/badge/license-BSD3-brightgreen)

A simple pushdown finite states machine library that separates data and behaviors as much as possible.

### In brief

1. States are just enum values.
2. Implement behaviors by inheriting from `pdfsm::B`
3. A behavior class shouldn't contain any internal data.
4. A StateMachine is just a struct, holding active states.
5. StateMachineHandler is for handling each state machine's behavior.

### Code Example

1. Defines state enums, there the trailing `N` is necessary:

   ```cpp
   enum class RobotState { Idle, Moving, Dancing, N };
   ```

2. Make a transition table:

   ```cpp
   pdfsm::TransitionTable<RobotState> transitions{
       {RobotState::Idle, {RobotState::Moving, RobotState::Dancing}},
       {RobotState::Moving, {RobotState::Idle, RobotState::Dancing}},
       {RobotState::Dancing, {RobotState::Idle}},
   };
   ```

2. Defines the state behavior class for each state:

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

3. Creates a state machine, it's just a struct holding active states in a static-array based stack:

   ```cpp
   pdfsm::StateMachine<RobotState> fsm;
   ```

4. Makes a context for ticking / update:

   ```cpp
   pdfsm::Context ctx;
   ```

5. Creates a handler to manipulate state transitions:

   ```cpp
   pdfsm::StateMachineHandler<RobotState> h(behaviors, transitions);
   ```

   Before handling a fsm struct, binds it:

   ```cpp
   h.SetHandlingFsm(fsm, ctx);
   ```

   And then, after handling, clears the binding:

   ```cpp
   h.ClearHandlingFsm();
   ```

6. We can access the handler inside state behavior classes:

   ```cpp
   auto& handler = GetHandler(); // returns a pointer to the handler.
   ```

7. Operations / APIs of the handler:

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
