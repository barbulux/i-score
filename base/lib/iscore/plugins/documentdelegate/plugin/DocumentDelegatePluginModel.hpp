#pragma once
#include <iscore/tools/NamedObject.hpp>
#include <QJsonObject>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModel.hpp>

namespace iscore
{
    // TODO : make it take a DocumentModel necessarily as parent.
    class DocumentDelegatePluginModel : public NamedObject
    {
            Q_OBJECT
        public:
            using NamedObject::NamedObject;
            virtual ~DocumentDelegatePluginModel() = default;

            virtual QString metadataName() const = 0;
            virtual ElementPluginModel* makeElementPlugin(const QString&,
                                                     QObject* parent) = 0;
            virtual ElementPluginModel* makeElementPlugin(const QString&,
                                                     SerializationIdentifier identifier,
                                                     void* data,
                                                     QObject* parent) = 0;

            virtual ElementPluginModel* cloneElementPlugin(iscore::ElementPluginModel*,
                                                              QObject* parent) = 0;

            virtual QWidget* makeElementPluginWidget(const ElementPluginModel*) const = 0;

            // TODO use the other way.
            virtual QJsonObject toJson() const = 0;
            virtual QByteArray toByteArray() const = 0;
    };

    class DocumentDelegatePluginModelFactory
    {
        public:
            virtual DocumentDelegatePluginModel* make(QVariant data, QObject* parent) = 0;
    };
}
