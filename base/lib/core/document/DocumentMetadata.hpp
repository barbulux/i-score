#pragma once
#include <QString>
#include <QObject>
#include <QDateTime>
#include <wobjectdefs.h>

#include <iscore_lib_base_export.h>
namespace iscore
{
struct ISCORE_LIB_BASE_EXPORT DocumentMetadata : public QObject
{
        W_OBJECT(DocumentMetadata)

        QString m_fileName{QObject::tr("Untitled")};
        QString m_author;
        QDateTime m_creation;
        QDateTime m_lastEdition;

    public:
        QString fileName() const;
        QString author() const;
        QDateTime creation() const;
        QDateTime lastEdition() const;

        void setFileName(QString fileName);
        void setAuthor(QString author);
        void setCreation(QDateTime creation);
        void setLastEdition(QDateTime lastEdition);

        //signals:
        void fileNameChanged(QString fileName)
        W_SIGNAL(fileNameChanged, fileName)

        void authorChanged(QString author)
        W_SIGNAL(authorChanged, author)

        void creationChanged(QDateTime creation)
        W_SIGNAL(creationChanged, creation)

        void lastEditionChanged(QDateTime lastEdition)
        W_SIGNAL(lastEditionChanged, lastEdition)

        private:
          W_PROPERTY(QString, fileName WRITE setFileName READ fileName NOTIFY fileNameChanged)
          W_PROPERTY(QString, author WRITE setAuthor READ author NOTIFY authorChanged)
          W_PROPERTY(QDateTime, creation WRITE setCreation READ creation NOTIFY creationChanged)
          W_PROPERTY(QDateTime, lastEdition WRITE setLastEdition READ lastEdition NOTIFY lastEditionChanged)

};

}
