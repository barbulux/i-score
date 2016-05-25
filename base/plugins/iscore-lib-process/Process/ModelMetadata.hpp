#pragma once
#include <Process/Style/ColorReference.hpp>
#include <QColor>
#include <qnamespace.h>
#include <QObject>
#include <QString>

#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore_lib_process_export.h>
/**
 * @brief The ModelMetadata class
 */
class ISCORE_LIB_PROCESS_EXPORT ModelMetadata : public QObject
{
        ISCORE_SERIALIZE_FRIENDS(ModelMetadata, DataStream)
        ISCORE_SERIALIZE_FRIENDS(ModelMetadata, JSONObject)

        Q_OBJECT
        Q_PROPERTY(QString name
                   READ name
                   WRITE setName
                   NOTIFY nameChanged)

        Q_PROPERTY(QString comment
                   READ comment
                   WRITE setComment
                   NOTIFY commentChanged)

        Q_PROPERTY(ColorRef color
                   READ color
                   WRITE setColor
                   NOTIFY colorChanged)

        Q_PROPERTY(QString label
                   READ label
                   WRITE setLabel
                   NOTIFY labelChanged)

    public:
        ModelMetadata() = default;
        ModelMetadata(const ModelMetadata& other) :
            QObject {}
        {
            setName(other.name());
            setComment(other.comment());
            setColor(other.color());
            setLabel(other.label());
        }

        ModelMetadata& operator= (const ModelMetadata& other)
        {
            setName(other.name());
            setComment(other.comment());
            setColor(other.color());
            setLabel(other.label());

            return *this;
        }

        const QString& name() const;
        const QString& comment() const;
        ColorRef color() const;
        const QString& label() const;

        void setName(const QString& arg);
        void setComment(const QString& arg);
        void setColor(ColorRef arg);
        void setLabel(const QString& arg);


    signals:
        void nameChanged(const QString& arg);
        void commentChanged(const QString& arg);
        void colorChanged(ColorRef arg);
        void labelChanged(const QString& arg);
        void metadataChanged();

    private:
        QString m_scriptingName;
        QString m_comment;
        ColorRef m_color;
        QString m_label;
};

W_REGISTER_ARGTYPE(ModelMetadata)

