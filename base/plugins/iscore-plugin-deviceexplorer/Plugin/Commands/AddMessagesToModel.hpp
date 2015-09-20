#pragma once
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <State/Message.hpp>
#include <DeviceExplorer/Node/DeviceExplorerNode.hpp>
namespace iscore
{
class MessageItemModel;
}

// On drop
class AddMessagesToModel : public iscore::SerializableCommand
{
        ISCORE_COMMAND_DECL("ScenarioControl", "AddMessagesToModel", "AddMessagesToModel")
    public:
        ISCORE_SERIALIZABLE_COMMAND_DEFAULT_CTOR(AddMessagesToModel)
        AddMessagesToModel(
        Path<iscore::MessageItemModel>&& path,
          const iscore::MessageList& messages);

        virtual void undo() override;
        virtual void redo() override;

    protected:
        virtual void serializeImpl(QDataStream&) const override;
        virtual void deserializeImpl(QDataStream&) override;

    private:
        Path<iscore::MessageItemModel> m_path;

        iscore::Node m_old;
        iscore::Node m_new;
};