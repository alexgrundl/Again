#ifndef PORTROLESELECTION_H
#define PORTROLESELECTION_H

#include <memory>

#include "statemachinebase.h"
#include "ptpmessageannounce.h"

class PortRoleSelection : public StateMachineBase
{
public:


    PortRoleSelection(TimeAwareSystem *timeAwareSystem, std::vector<INetPort*> ports);


    virtual ~PortRoleSelection();

    /**
     * @brief Sets all the elements of the selectedRole array (see 10.2.3.20) to DisabledPort. Sets lastGmPriority to all ones.
     * Sets the pathTrace array (see 10.3.8.21) to contain the single element thisClock (see 10.2.3.22).
     */
    void UpdtRoleDisabledTree();

    /**
     * @brief Sets all the elements of the reselect array (see 10.3.8.1) to false.
     */
    void ClearReselectTree();

    /**
     * @brief Sets all the elements of the selected array (see 10.3.8.2) to true.
     */
    void SetSelectedTree();

    /**
     * @brief UpdtRolesTree
     */
    void UpdtRolesTree();

    void ProcessState();


private:

    std::vector<INetPort*> m_ports;


    const char* GetStrPortRole(PortRole role);
};

#endif // PORTROLESELECTION_H
