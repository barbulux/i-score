#pragma once
#include <OSSIA/Executor/ClockManager/ClockManagerFactory.hpp>
#include <Editor/TimeConstraint.h>
namespace Scenario
{ class ConstraintModel; }
namespace RecreateOnPlay
{
class BaseScenarioElement;
class ISCORE_PLUGIN_OSSIA_EXPORT DefaultClockManager final : public ClockManager
{
    public:
        DefaultClockManager(const Context& ctx);

        virtual ~DefaultClockManager();

        // Pass the root constraint.
        static OSSIA::TimeConstraint::ExecutionCallback makeDefaultCallback(
                RecreateOnPlay::BaseScenarioElement&);
    private:
        void play_impl(
                const TimeValue& t,
                BaseScenarioElement&) override;
        void pause_impl(BaseScenarioElement&) override;
        void resume_impl(BaseScenarioElement&) override;
        void stop_impl(BaseScenarioElement&) override;
};

class ISCORE_PLUGIN_OSSIA_EXPORT DefaultClockManagerFactory final : public ClockManagerFactory
{
        ISCORE_CONCRETE_FACTORY("583e9c52-e136-46b6-852f-7eef2993e9eb")

    public:
        virtual ~DefaultClockManagerFactory();
        QString prettyName() const override;
        std::unique_ptr<ClockManager> make(
            const RecreateOnPlay::Context& ctx) override;
};

}
