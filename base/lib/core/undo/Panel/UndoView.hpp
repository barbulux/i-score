#pragma once
#include <iscore/plugins/panel/PanelView.hpp>
namespace iscore
{
class CommandStack;
class UndoListWidget;
}

class UndoView : public iscore::PanelView
{
    public:
        const iscore::DefaultPanelStatus& defaultPanelStatus() const override;

        UndoView(QObject *v);

        QWidget *getWidget() override;
        void setStack(iscore::CommandStack *s);

    private:
        iscore::UndoListWidget *m_list{};
        QWidget *m_widget{};
};
