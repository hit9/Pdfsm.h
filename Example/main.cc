#include <iostream>

#include "Pdfsm.h"

// States
enum class RobotState
{
	Idle,
	Moving,
	Dancing,
	N
};

// Transition Table
Pdfsm::TransitionTable<RobotState> transitions{
	{ RobotState::Idle, { RobotState::Moving, RobotState::Dancing } },
	{ RobotState::Moving, { RobotState::Idle, RobotState::Dancing } },
	{ RobotState::Dancing, { RobotState::Idle } },
};

// State Behaviors
class RobotIdleBehavior : public Pdfsm::B<RobotState::Idle>
{
public:
	void OnSetup() override { std::cout << "Idle: on setup" << std::endl; }
	void OnEnter(const Pdfsm::Context& ctx) override
	{
		std::cout << "Idle: on enter" << std::endl;
	}
	void OnTerminate(const Pdfsm::Context& ctx) override
	{
		std::cout << "Idle: on terminate" << std::endl;
	}
	void OnPause(const Pdfsm::Context& ctx) override
	{
		std::cout << "Idle: on paused" << std::endl;
	}
	void OnResume(const Pdfsm::Context& ctx) override
	{
		std::cout << "Idle: on resumed" << std::endl;
	}
	void Update(const Pdfsm::Context& ctx) override
	{
		std::cout << "Idle: on update" << std::endl;
	}
};

class RobotMovingBehavior : public Pdfsm::B<RobotState::Moving>
{
	void OnSetup() override { std::cout << "moving: on setup" << std::endl; }
	void OnEnter(const Pdfsm::Context& ctx) override
	{
		std::cout << "moving: on enter" << std::endl;
	}
	void OnTerminate(const Pdfsm::Context& ctx) override
	{
		std::cout << "moving: on terminate" << std::endl;
	}
	void OnPause(const Pdfsm::Context& ctx) override
	{
		std::cout << "moving: on paused" << std::endl;
	}
	void OnResume(const Pdfsm::Context& ctx) override
	{
		std::cout << "moving: on resumed" << std::endl;
	}
	void Update(const Pdfsm::Context& ctx) override
	{
		std::cout << "moving: on update" << std::endl;
	}
};

class RobotDancingBehavior : public Pdfsm::B<RobotState::Dancing>
{
	void OnSetup() override { std::cout << "dancing: on setup" << std::endl; }
	void OnEnter(const Pdfsm::Context& ctx) override
	{
		std::cout << "dancing: on enter" << std::endl;
	}
	void OnTerminate(const Pdfsm::Context& ctx) override
	{
		std::cout << "dancing: on terminate" << std::endl;
	}
	void OnPause(const Pdfsm::Context& ctx) override
	{
		std::cout << "dancing: on paused" << std::endl;
	}
	void OnResume(const Pdfsm::Context& ctx) override
	{
		std::cout << "dancing: on resumed" << std::endl;
	}
	void Update(const Pdfsm::Context& ctx) override
	{
		std::cout << "dancing: on update" << std::endl;
	}
};

Pdfsm::BTable<RobotState> behaviors{
	std::make_unique<RobotIdleBehavior>(),
	std::make_unique<RobotMovingBehavior>(),
	std::make_unique<RobotDancingBehavior>(),
};

Pdfsm::Context ctx;

Pdfsm::StateMachine<RobotState> fsm;

int main(void)
{
	Pdfsm::StateMachineHandler<RobotState> h(behaviors, transitions);

	h.SetHandlingFsm(fsm, ctx);
	h.Jump(ctx, RobotState::Moving);
	h.Push(ctx, RobotState::Dancing);
	h.Pop(ctx);
	h.ClearHandlingFsm();
	return 0;
}
