/*
 * NimBLEServer.h
 *
 *  Created: on March 2, 2020
 *      Author H2zero
 * 
 * Originally:
 *
 * BLEServer.h
 *
 *  Created on: Apr 16, 2017
 *      Author: kolban
 */

#ifndef MAIN_NIMBLESERVER_H_
#define MAIN_NIMBLESERVER_H_
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)
//#include <esp_gatts_api.h>

// #include "BLEDevice.h"
#include "NimBLEAddress.h"
#include "NimBLEUUID.h"
#include "NimBLEAdvertising.h"
//#include "NimBLECharacteristic.h"
#include "NimBLEService.h"
#include "NimBLESecurity.h"
#include "FreeRTOS.h"


#include <map>

class NimBLEService;
class NimBLECharacteristic;
class NimBLEServerCallbacks;
/* TODO possibly refactor this struct */ 
typedef struct {
    void *peer_device;      // peer device BLEClient or BLEServer - maybe its better to have 2 structures or union here
    bool connected;         // do we need it?
    uint16_t mtu;           // every peer device negotiate own mtu
} conn_status_t;


/**
 * @brief A data structure that manages the %BLE servers owned by a BLE server.
 */
class NimBLEServiceMap {
public:
//    NimBLEService* getByHandle(uint16_t handle);
    NimBLEService* getByUUID(const char* uuid);    
    NimBLEService* getByUUID(NimBLEUUID uuid, uint8_t inst_id = 0);
//    void           handleGATTServerEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param);
//    void           setByHandle(uint16_t handle, NimBLEService* service);
    void           setByUUID(const char* uuid, NimBLEService* service);
    void           setByUUID(NimBLEUUID uuid, NimBLEService* service);
    std::string    toString();
    NimBLEService* getFirst();
    NimBLEService* getNext();
    void           removeService(NimBLEService *service);
    int            getRegisteredServiceCount();

private:
//    std::map<uint16_t, NimBLEService*>    m_handleMap;
    std::map<NimBLEService*, std::string> m_uuidMap;
    std::map<NimBLEService*, std::string>::iterator m_iterator;
};


/**
 * @brief The model of a %BLE server.
 */
class NimBLEServer {
public:
    uint32_t              getConnectedCount();
    NimBLEService*        createService(const char* uuid);    
    NimBLEService*        createService(NimBLEUUID uuid, uint32_t numHandles=15, uint8_t inst_id=0);
    NimBLEAdvertising*    getAdvertising();
    void                  setCallbacks(NimBLEServerCallbacks* pCallbacks);
    void                  startAdvertising();
    void                  stopAdvertising();
    void                  start();
//    void                  removeService(BLEService* service);
    NimBLEService*        getServiceByUUID(const char* uuid);
    NimBLEService*        getServiceByUUID(NimBLEUUID uuid);
    int                   disconnect(uint16_t connID, uint8_t reason = BLE_ERR_REM_USER_CONN_TERM);
    NimBLECharacteristic* getChrByHandle(uint16_t handle);
//    bool                connect(BLEAddress address);
//    uint16_t            m_appId;
//    void                updateConnParams(esp_bd_addr_t remote_bda, uint16_t minInterval, uint16_t maxInterval, uint16_t latency, uint16_t timeout);

    /* multi connection support */
    std::map<uint16_t, conn_status_t> getPeerDevices();
    void          addPeerDevice(void* peer, bool is_client, uint16_t conn_id);
    void          removePeerDevice(uint16_t conn_id, bool client);
    NimBLEServer* getServerByConnId(uint16_t conn_id);
    void          updatePeerMTU(uint16_t connId, uint16_t mtu);
    uint16_t      getPeerMTU(uint16_t conn_id);
    uint16_t      getConnId();


private:
    NimBLEServer();
    //friend class BLEService;
    friend class NimBLECharacteristic;
    friend class NimBLEDevice;
    friend class NimBLEAdvertising;
    //esp_ble_adv_data_t  m_adv_data;
    // BLEAdvertising      m_bleAdvertising;
    uint16_t               m_connId;
    uint16_t               m_svcChgChrHdl;
    std::map<uint16_t, conn_status_t> m_connectedServersMap;

    //FreeRTOS::Semaphore m_semaphoreRegisterAppEvt   = FreeRTOS::Semaphore("RegisterAppEvt");
    //FreeRTOS::Semaphore m_semaphoreCreateEvt        = FreeRTOS::Semaphore("CreateEvt");
    //FreeRTOS::Semaphore m_semaphoreOpenEvt          = FreeRTOS::Semaphore("OpenEvt");
    NimBLEServiceMap       m_serviceMap;
    NimBLEServerCallbacks* m_pServerCallbacks = nullptr;

    //void            createApp(uint16_t appId);
    //uint16_t        getGattsIf();
    //void            handleGATTServerEvent(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
	static int 		handleGapEvent(struct ble_gap_event *event, void *arg);
    //void            registerApp(uint16_t);
}; // NimBLEServer


/**
 * @brief Callbacks associated with the operation of a %BLE server.
 */
class NimBLEServerCallbacks {
public:
    virtual ~NimBLEServerCallbacks() {};
    /**
     * @brief Handle a new client connection.
     *
     * When a new client connects, we are invoked.
     *
     * @param [in] pServer A reference to the %BLE server that received the client connection.
     */
    virtual void onConnect(NimBLEServer* pServer);
    //virtual void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);
    /**
     * @brief Handle an existing client disconnection.
     *
     * When an existing client disconnects, we are invoked.
     *
     * @param [in] pServer A reference to the %BLE server that received the existing client disconnection.
     */
    virtual void onDisconnect(NimBLEServer* pServer);
}; // BLEServerCallbacks


#endif /* CONFIG_BT_ENABLED */
#endif /* MAIN_NIMBLESERVER_H_ */