#include "statemachinemanager.h"

StateMachineManager::StateMachineManager()
{
    m_stateThread = new CThreadWrapper<StateMachineManager>(this, &StateMachineManager::Process, std::string("State machine thread"));
}

StateMachineManager::~StateMachineManager()
{
    m_stateThread->Stop();
    delete m_stateThread;
    m_stateThread = NULL;
}

void StateMachineManager::StartProcessing()
{
    m_stateThread->Start();
}

void StateMachineManager::SetStateMachines(std::vector<StateMachineBase*>& stateMachines)
{
    m_stateMachines = stateMachines;
}

uint32_t StateMachineManager::Process(bool_t* pbIsRunning, pal::EventHandle_t pWaitHandle)
{
    pal::EventWaitResult_e dwWaitResult;

    InitialProcess();
    while(*pbIsRunning)
    {
      dwWaitResult = pal::EventWait(pWaitHandle, m_granularity_ms);
      if(dwWaitResult == pal::EventWaitTimeout)
      {
          for (std::vector<StateMachineBase>::size_type i = 0; i < m_stateMachines.size(); ++i)
          {
              m_stateMachines[i]->ProcessState();
          }
      }
    }

    return 0;
}


void StateMachineManager::InitialProcess()
{
    for (std::vector<StateMachineBase>::size_type i = 0; i < m_stateMachines.size(); ++i)
    {
        m_stateMachines[i]->ProcessState();
    }
    if(m_stateMachines.size() > 0)
        m_stateMachines[0]->GetTimeAwareSystem()->BEGIN = false;
}
