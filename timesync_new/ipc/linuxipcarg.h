#ifndef LINUXIPCARG_H
#define LINUXIPCARG_H

#include "avbts_osipc.hpp"

/**
 * @brief Extends IPC ARG generic interface to linux
 */
class LinuxIPCArg : public OS_IPC_ARG {
private:
    std::string group_name;
    std::string if_name;
    unsigned char domain;
public:
    /**
     * @brief  Initializes IPCArg object
     */
    LinuxIPCArg( ) {
    }

    /**
     * @brief  Sets the group name
     * @param group_name [in] Group's name
     */
    void setGroupName(const char *group_name) {
        this->group_name = group_name;
    }

    /**
     * @brief  Sets the interface name (e.g. eth1) and the domain number
     * @param if_name [in] The interface name (e.g. eth1)
     * @param domain [in] The domain number
     */
    void setIfnameAndDomain(const char* if_name, unsigned char domain) {
        this->if_name = if_name;
        this->domain = domain;
    }

    /**
     * @brief Destroys IPCArg internal variables
     */
    virtual ~LinuxIPCArg() {
    }
    friend class LinuxSharedMemoryIPC;
};

#endif // LINUXIPCARG_H
