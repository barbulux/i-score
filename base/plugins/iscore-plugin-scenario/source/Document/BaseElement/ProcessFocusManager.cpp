#include "ProcessFocusManager.hpp"


const ProcessModel* ProcessFocusManager::focusedModel()
{
    return m_currentModel;
}


const ProcessViewModel* ProcessFocusManager::focusedViewModel()
{
    return m_currentViewModel;
}


ProcessPresenter* ProcessFocusManager::focusedPresenter()
{
    return m_currentPresenter;
}


void ProcessFocusManager::setFocusedPresenter(ProcessPresenter* p)
{
    if(p == m_currentPresenter)
        return;

    if(m_currentPresenter)
    {
        defocusPresenter(m_currentPresenter);
    }
    if(m_currentViewModel)
    {
        emit sig_defocusedViewModel(m_currentViewModel);
    }

    m_currentPresenter = p;

    if(m_currentPresenter)
    {
        m_currentViewModel = &m_currentPresenter->viewModel();
        m_currentModel = &m_currentViewModel->sharedProcessModel();

        emit sig_focusedViewModel(m_currentViewModel);

        focusPresenter(m_currentPresenter);
    }
    else
    {
        m_currentViewModel = nullptr;
        m_currentModel = nullptr;
    }
}


void ProcessFocusManager::focusNothing()
{
    if(m_currentViewModel)
        emit sig_defocusedViewModel(m_currentViewModel);
    if(m_currentPresenter)
        defocusPresenter(m_currentPresenter);

    m_currentModel = nullptr;
    m_currentViewModel = nullptr;
    m_currentPresenter = nullptr;
}

void ProcessFocusManager::focusPresenter(ProcessPresenter* p)
{
    p->setFocus(true);
    emit sig_focusedPresenter(p);
}

void ProcessFocusManager::defocusPresenter(ProcessPresenter* p)
{
    p->setFocus(false);
    emit sig_defocusedPresenter(p);
}
