#include <iostream>

#include "pdfsm.h"

// States
enum class RobotState { Idle, Moving, Dancing, N };

// Transition Table
pdfsm::TransitionTable<RobotState> transitions{
    {RobotState::Idle, {RobotState::Moving, RobotState::Dancing}},
    {RobotState::Moving, {RobotState::Idle, RobotState::Dancing}},
    {RobotState::Dancing, {RobotState::Idle}},
};

// State Behaviors
class RobotIdleBehavior : public pdfsm::B<RobotState::Idle> {
 public:
  void OnSetup() override { std::cout << "Idle: on setup" << std::endl; }
  void OnEnter(const pdfsm::Context& ctx) override {
    std::cout << "Idle: on enter" << std::endl;
  }
  void OnTerminate(const pdfsm::Context& ctx) override {
    std::cout << "Idle: on terminate" << std::endl;
  }
  void OnPause(const pdfsm::Context& ctx) override {
    std::cout << "Idle: on paused" << std::endl;
  }
  void OnResume(const pdfsm::Context& ctx) override {
    std::cout << "Idle: on resumed" << std::endl;
  }
  void Update(const pdfsm::Context& ctx) override {
    std::cout << "Idle: on update" << std::endl;
  }
};

class RobotMovingBehavior : public pdfsm::B<RobotState::Moving> {
  void OnSetup() override { std::cout << "moving: on setup" << std::endl; }
  void OnEnter(const pdfsm::Context& ctx) override {
    std::cout << "moving: on enter" << std::endl;
  }
  void OnTerminate(const pdfsm::Context& ctx) override {
    std::cout << "moving: on terminate" << std::endl;
  }
  void OnPause(const pdfsm::Context& ctx) override {
    std::cout << "moving: on paused" << std::endl;
  }
  void OnResume(const pdfsm::Context& ctx) override {
    std::cout << "moving: on resumed" << std::endl;
  }
  void Update(const pdfsm::Context& ctx) override {
    std::cout << "moving: on update" << std::endl;
  }
};

class RobotDancingBehavior : public pdfsm::B<RobotState::Dancing> {
  void OnSetup() override { std::cout << "dancing: on setup" << std::endl; }
  void OnEnter(const pdfsm::Context& ctx) override {
    std::cout << "dancing: on enter" << std::endl;
  }
  void OnTerminate(const pdfsm::Context& ctx) override {
    std::cout << "dancing: on terminate" << std::endl;
  }
  void OnPause(const pdfsm::Context& ctx) override {
    std::cout << "dancing: on paused" << std::endl;
  }
  void OnResume(const pdfsm::Context& ctx) override {
    std::cout << "dancing: on resumed" << std::endl;
  }
  void Update(const pdfsm::Context& ctx) override {
    std::cout << "dancing: on update" << std::endl;
  }
};

pdfsm::BTable<RobotState> behaviors{
    std::make_unique<RobotIdleBehavior>(),
    std::make_unique<RobotMovingBehavior>(),
    std::make_unique<RobotDancingBehavior>(),
};

pdfsm::Context ctx;

pdfsm::StateMachine<RobotState> fsm;

int main(void) {
  pdfsm::StateMachineHandler<RobotState> h(behaviors, transitions);

  h.SetupHandlingFsm(fsm);
  h.Jump(ctx, RobotState::Moving);
  h.Push(ctx, RobotState::Dancing);
  h.Pop(ctx);
  h.ClearHandlingFsm();
  return 0;
}
