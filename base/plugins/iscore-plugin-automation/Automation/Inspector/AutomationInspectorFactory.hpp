#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

class InspectorWidgetBase;
class QObject;
class QString;
class QWidget;
namespace iscore {
struct DocumentContext;
}  // namespace iscore

namespace Automation
{
class InspectorFactory final : public Process::InspectorWidgetDelegateFactory
{
        ISCORE_CONCRETE_FACTORY("2c5410ba-d3df-45b8-8444-6d8578b4e1a8")
    public:
        InspectorFactory() = default;

    private:
        Process::InspectorWidgetDelegate* make(
                const Process::ProcessModel&,
                const iscore::DocumentContext& doc,
                QWidget* parent) const override;
        bool matches(const Process::ProcessModel&) const override;

};
}
