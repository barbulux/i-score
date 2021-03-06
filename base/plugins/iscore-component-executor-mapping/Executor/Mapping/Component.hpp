#pragma once
#include <OSSIA/Executor/ProcessElement.hpp>

#include <State/Address.hpp>
#include <Mapping/MappingModel.hpp>
#include <QPointer>
#include <Editor/Value/Value.h>
namespace OSSIA
{
    class Mapper;
    class CurveAbstract;
}
namespace Device
{
class DeviceList;
}

namespace Mapping
{
namespace RecreateOnPlay
{
class Component final :
        public ::RecreateOnPlay::ProcessComponent_T<Mapping::ProcessModel>
{
        COMPONENT_METADATA("da360b58-9885-4106-be54-8e272ed45dbe")
    public:
        Component(
                ::RecreateOnPlay::ConstraintElement& parentConstraint,
                ::Mapping::ProcessModel& element,
                const ::RecreateOnPlay::Context& ctx,
                const Id<iscore::Component>& id,
                QObject* parent);

    private:
        void recreate();
        std::shared_ptr<OSSIA::CurveAbstract> rebuildCurve();

        OSSIA::Type m_sourceAddressType{OSSIA::Type(-1)};
        OSSIA::Type m_targetAddressType{OSSIA::Type(-1)};

        template<typename T>
        std::shared_ptr<OSSIA::CurveAbstract> on_curveChanged_impl();

        template<typename X_T, typename Y_T>
        std::shared_ptr<OSSIA::CurveAbstract> on_curveChanged_impl2();

        std::shared_ptr<OSSIA::CurveAbstract> m_ossia_curve;

        const Device::DeviceList& m_deviceList;
};

using ComponentFactory = ::RecreateOnPlay::ProcessComponentFactory_T<Component>;

}
}

ISCORE_COMPONENT_FACTORY(RecreateOnPlay::ProcessComponentFactory, Mapping::RecreateOnPlay::ComponentFactory)
