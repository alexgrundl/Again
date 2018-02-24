#ifndef STATEMACHINEMANAGER_H
#define STATEMACHINEMANAGER_H

#include <vector>

#include "c-extensions/ThreadWrapper.h"
#include "statemachinebase.h"

class StateMachineManager
{
public:

    StateMachineManager();


    ~StateMachineManager();


    void StartProcessing();


    void SetStateMachines(std::vector<StateMachineBase*>& stateMachines);

private:

    static const uint32_t m_granularity_ms = 10;


    uint32_t Process(bool_t *pbIsRunning, pal::EventHandle_t pWaitHandle);


    CThreadWrapper<StateMachineManager>* m_stateThread;


    std::vector<StateMachineBase*> m_stateMachines;


    void InitialProcess();
};

#endif // STATEMACHINEMANAGER_H
