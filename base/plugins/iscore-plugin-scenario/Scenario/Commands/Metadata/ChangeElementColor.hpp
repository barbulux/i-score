#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>

#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Process/Style/ColorReference.hpp>

namespace Scenario
{
namespace Command
{
template<class T>
class ChangeElementColor final : public iscore::SerializableCommand
{
        // No ISCORE_COMMAND here since it's a template.
    public:
        const CommandParentFactoryKey& parentKey() const override
        {
            return ScenarioCommandFactoryName();
        }
        static const CommandFactoryKey& static_key()
        {
            auto name = QString("ChangeElementColor_")  + Metadata<ObjectKey_k, T>::get();
            static const CommandFactoryKey kagi{std::move(name)};
            return kagi;
        }
        const CommandFactoryKey& key() const override
        {
            return static_key();
        }
        QString description() const override
        {
            return QObject::tr("Change %1 color").arg(Metadata<Description_k, T>::get());
        }

        ChangeElementColor() = default;
        ChangeElementColor(Path<T>&& path, ColorRef newLabel) :
            m_path {std::move(path) },
            m_newColor {newLabel}
        {
            auto& obj = m_path.find();
            m_oldColor = obj.metadata.color();
        }

        void undo() const override
        {
            auto& obj = m_path.find();
            obj.metadata.setColor(m_oldColor);
        }

        void redo() const override
        {
            auto& obj = m_path.find();
            obj.metadata.setColor(m_newColor);
        }

    protected:
        void serializeImpl(DataStreamInput& s) const override
        {
            s << m_path << m_oldColor << m_newColor;
        }

        void deserializeImpl(DataStreamOutput& s) override
        {
            s >> m_path >> m_oldColor >> m_newColor;
        }

    private:
        Path<T> m_path;
        ColorRef m_newColor;
        ColorRef m_oldColor;
};
}
}

ISCORE_COMMAND_DECL_T(ChangeElementColor<ConstraintModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<EventModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<TimeNodeModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<StateModel>)
ISCORE_COMMAND_DECL_T(ChangeElementColor<Process::ProcessModel>)
