/****************************************************************
* Copyright (C) 2016 Andrey Krekhov, David McCann
*
* The content of this file may not be copied and/or distributed
* without the expressed permission of the copyright owner.
*
****************************************************************/

/**
 * @file Gate.h
 * @brief This header file contains the public interface of the MorphableUI Gate.
 * @date 02/16/2016
 * @version 1.0-rc1
 */

/** \mainpage Usage
* \section configuration Configuration
* There following options are available for configuration:
* - Server endpoint: IP and port of the server's TCP endpoint (not WebSocket).
* - Port for connections from the server: Either a port number or 'auto'. 'auto' lets the gate choose an available port by itself.
* - Port for connection from other gates: Either a port number or 'auto'. 'auto' lets the gate choose an available port by itself.
* - Password: Password for registration at the server, if the server is configured to require a password. Leave empty if no password is
required.
*
* Use MUI_initialize() to configure the gate.
* \section registration Registration
* After the gate has been configured, it should be registered at the server using MUI_registerGate(). Then, devices and applications hosted
* by the gate should be registered using MUI_registerDevice() and MUI_registerApplication(), respectively. **Caution:** In order to register
* a device or an application, the corresponding device type or application type must be known to the server. In order to register device
* types or application types, use MUI_registerDeviceTypes() or MUI_registerApplicationTypes(), respectively.
*
* \section interaction_events Interaction Events
* An interaction event can be conceived of as a key-value map. Keys are strings which identify the purpose of a value. E.g., an interaction
* event for 3D positional data might contain the keys "x", "y", "z", whereas an interaction event for 3D rotational data might contain the
* keys "roll", "pitch", "yaw". A value has one of the following data types:
* - MUI_INT32: 32-bit signed integer value.
* - MUI_FLOAT: 32-bit single precision floating point value.
* - MUI_BOOL: Boolean value.
* - MUI_STRING: String value. **Caution:** Currently, no specific encoding is enforced upon sender and receiver. The receiver will receive
the string as it is sent by the sender.
*
* Use MUI_createEvent() to create an interaction event and MUI_destroyEvent() to destroy an event which is no longer used. __Caution:__
Interaction events must always be destroyed when they are no longer used, otherwise memory is leaked. Use MUI_eventNumKeys() and
MUI_eventKeyByIndex() to get an event's keys. Use MUI_eventGetStringValue(), MUI_eventGetIntValue(), MUI_eventGetFloatValue(), and
MUI_eventGetBoolValue() to get the value corresponding to a key. Use MUI_eventSetStringValue(), MUI_eventSetIntValue(),
MUI_eventSetFloatValue(), and MUI_eventSetBoolValue() to add a new key-value pair with a value of the specified type.
*
* \section sending Sending Interaction Events
* If the capability of a device hosted by the gate is wired to a requrirement of some application via a UI, interaction events can be sent
to that application. To send an interaction event:
* - Create the event using MUI_createEvent(),
* - set the event's data using MUI_eventSetStringValue(), MUI_eventSetIntValue(), MUI_eventSetFloatValue(), and MUI_eventSetBoolValue(),
* - send the event using MUI_sendEvent(),
* - destroy the event using MUI_destroyEvent().
*
* **Caution:** If the capability passed to MUI_sendEvent() is not wired to an requirement, the call returns MUI_NO_RECEIVER, and no event is sent.
*
* \section receiving Receiving Interaction Events
* If one or more requirements of an application hosted by the gate are wired to the capability of some device via a UI, interaction events
can be received for processing by that application. To receive an interaction event:
* - Receive the event using MUI_pollEvent() or MUI_waitEvent(),
* - read the event's data using MUI_eventGetStringValue(), MUI_eventGetIntValue(), MUI_eventGetFloatValue(), and MUI_eventGetBoolValue(),
* - destroy the event using MUI_destroyEvent().
*
* The difference between MUI_pollEvent() and MUI_waitEvent() is that the former returns immediately when no event is available, whereas the
latter waits up to a specified amount of time until an event is available. Use the former when an application needs to proceed even in the
absence of user input (e.g., in the game loop of a game). **Caution:** If no event is available, MUI_pollEvent() and MUI_waitEvent() return
MUI_NO_EVENT and no event is returned. A caller must ensure that an event has been returned before trying to read data from it.
*/

#pragma once

#if defined(_WIN32) && !defined(__GNUC__)
// Windows
#ifdef gate_shared_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#else
//  Unix
#if defined(gate_shared_EXPORTS)
#define DLLEXPORT __attribute__((visibility("default")))
#else
#define DLLEXPORT
#endif
#endif

#include "Types.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
        Setup and Termination
*******************************************************/
/** @defgroup setup_group Setup and Termination
*  Functions relating to initialization, configuration, and termination of the gate.
*  @{
*/

/**
 * @brief Initializes and configures the gate.
 * @param[in] configuration The gate's global configuration.
 * @retval MUI_SUCCESS Returned if no error occured.
 * @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
 *
 * This function must be called before any other function is called.
 */
DLLEXPORT MUI_ERRCODE MUI_initialize(MUI_GateConfiguration configuration);

/**
* @brief Terminates the gate.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* After this function has been called, no other function except MUI_initialize() may be called.
*/
DLLEXPORT MUI_ERRCODE MUI_terminate();

/** @} */ // end of setup_group

/******************************************************
        Registration
*******************************************************/
/** @defgroup registration_group Registration
*  Functions relating to registration of gates, devices, applications, and types.
*  @{
*/

/**
* @brief Registers the gate at the server.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* A gate must be registered at the server before it can register devices, application, device types, or application types.
*/
DLLEXPORT MUI_ERRCODE MUI_registerGate();

/**
* @brief Registers a device of the specified type at the server.
* @param[in] deviceTypeID A string identifying the type of the device to be registered. The device type must be known to the server prior to
* a
* registration request.
* @param[in] deviceID A string identifying the device to be registered. The device ID must be unique.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* After a device has been registered, the server will consider the device and its capabilities in the calculation of UI proposals.
*/
DLLEXPORT MUI_ERRCODE MUI_registerDevice(const char* deviceTypeID, const char* deviceID);

/**
* @brief Registers an application of the specified type at the server.
* @param[in] applicationTypeID A string identifying the type of the application to be registered. The application type must be known to the
* server prior to a registration request.
* @param[in] applicationID A string identifying the application to be registered. The application ID must be unique.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* After an application has been registered, the server can generate UI proposals for this application.
*/
DLLEXPORT MUI_ERRCODE MUI_registerApplication(const char* applicationTypeID, const char* applicationID);

/**
* @brief Registers one or multiple device types at the server. The device type definitions are read from JSON files located at the specified
* path.
* @param[in] path If path denotes a JSON file, the device type defined by that file will be registered. If path denotes a directory, every
* JSON
* file in that directory is considered to contain a device type definition and each type is registered.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_registerDeviceTypes(const char* path);

/**
* @brief Registers one or multiple application types at the server. The application type definitions are read from JSON files located at the
* specified
* path.
* @param[in] path If path denotes a JSON file, the application type defined by that file will be registered. If path denotes a directory,
* every JSON
* file in that directory is considered to contain an application type definition and each type is registered.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_registerApplicationTypes(const char* path);

/** @} */ // end of registration_group


/******************************************************
        Creating and Inspecting Interaction Events
*******************************************************/
/** @defgroup events_group Creating and Inspecting Interaction Events
*  Functions relating to creation and inspection of interaction events.
*  @{
*/

/**
* @brief Creates a new interaction event. The event is accessed through a handle.
* @param[out] eventHandle A handle to the created interaction event.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_createEvent(MUI_HEVENT* eventHandle);

/**
* @brief Destroys an interaction event and frees the associated memory.
* @param[in] eventHandle A handle to the interaction event.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* An event created by MUI_createEvent(), MUI_pollEvent(), or MUI_waitEvent() must be destroyed when it is no longer needed. Otherwise memory
* is
* leaked.
*/
DLLEXPORT MUI_ERRCODE MUI_destroyEvent(MUI_HEVENT eventHandle);

/**
* @brief Gets the number of keys the interaction event contains.
* @param[in] eventHandle A handle to the interaction event.
* @param[out] numKeys The number of keys the interaction event contains.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventNumKeys(MUI_HEVENT eventHandle, uint32_t* numKeys);

/**
* @brief Gets the interaction event's key at the specified index.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] index The index of the key to be returned. The first index is 0.
* @param[out] buffer The buffer in which the key will be returned. If the buffer is too small, the key will be truncated.
* @param[inout] bufferSize The size of the buffer. If the function call returns successfully, bufferSize will contain the actual size of the
* key (if the key fits in the buffer) or the required buffer size (if the key does not fit in the buffer).
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* Use MUI_eventNumKeys() to get the number of keys the interaction event contains.
*/
DLLEXPORT MUI_ERRCODE MUI_eventKeyByIndex(MUI_HEVENT eventHandle, uint32_t index, char* buffer, uint16_t* bufferSize);

/**
* @brief Adds a key-value pair with a string value to the interaction event.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key to be added.
* @param[in] value The value to be added.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventSetStringValue(MUI_HEVENT eventHandle, const char* key, const char* value);

/**
* @brief Adds a key-value pair with an integer value to the interaction event.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key to be added.
* @param[in] value The value to be added.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventSetIntValue(MUI_HEVENT eventHandle, const char* key, int32_t value);

/**
* @brief Adds a key-value pair with a floating point value to the interaction event.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key to be added.
* @param[in] value The value to be added.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventSetFloatValue(MUI_HEVENT eventHandle, const char* key, float value);

/**
* @brief Adds a key-value pair with a boolean value to the interaction event.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key to be added.
* @param[in] value The value to be added.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventSetBoolValue(MUI_HEVENT eventHandle, const char* key, bool value);


/**
* @brief Gets the value type for the specified key.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key identifying the value for which the type is to be determined.
* @param[out] type The value type.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetValueType(MUI_HEVENT eventHandle, const char* key, MUI_EventValueType* type);

/**
* @brief Gets the size of a value of type string.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key identifying the value for which the string size is to be determined.
* @param[out] stringSize The size of the string value.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*
* Call this function before calling MUI_eventGetStringValue() in order to determine the required buffer size.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetStringValueSize(MUI_HEVENT eventHandle, const char* key, uint16_t* stringSize);

/**
* @brief Gets a value of type string.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key of the value to be returned.
* @param[out] buffer The buffer in which the value will be returned. If the buffer is too small, the value will be truncated.
* @param[inout] bufferSize The size of the buffer. If the function call returns successfully, bufferSize will contain the actual size of the
* value (if the value fits in the buffer) or the required buffer size (if the value does not fit in the buffer).
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetStringValue(MUI_HEVENT eventHandle, const char* key, char* buffer, uint16_t* bufferSize);

/**
* @brief Gets a value of type integer.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key of the value to be returned.
* @param[out] value The returned value.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetIntValue(MUI_HEVENT eventHandle, const char* key, int32_t* value);

/**
* @brief Gets a value of type float.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key of the value to be returned.
* @param[out] value The returned value.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetFloatValue(MUI_HEVENT eventHandle, const char* key, float* value);

/**
* @brief Gets a value of type bool.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] key The key of the value to be returned.
* @param[out] value The returned value.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
*/
DLLEXPORT MUI_ERRCODE MUI_eventGetBoolValue(MUI_HEVENT eventHandle, const char* key, bool* value);

/** @} */ // end of events_group


/******************************************************
        Sending and Receiving Interaction Events
******************************************************/
/** @defgroup sendreceive_group Sending and Receiving Interaction Events
*  Functions relating to sending and receiving of interaction events.
*  @{
*/

/**
* @brief Sends an interaction event as originating from the specified capability of the specified device.
* @param[in] eventHandle A handle to the interaction event.
* @param[in] deviceID The device ID of the device from which the event originates.
* @param[in] capabilityID The capability ID of the device's capability from which the event originates.
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
* @retval MUI_NO_RECEIVER Returned if the combination of device ID and capability ID isn't wired to a receiver in a UI.
*
* After the call returns, the interaction event should be destroyed using MUI_destroyEvent().
*/
DLLEXPORT MUI_ERRCODE MUI_sendEvent(MUI_HEVENT eventHandle, const char* deviceID, const char* capabilityID);

/**
* @brief Poll the gate for interaction events destined for the specified application.
* @param[out] eventHandle A handle to the interaction event, if an event is recieved.
* @param[in] applicationID The application ID for which an event should be received.
* @param[out] requirementBuffer A buffer in which the requirement ID of the received event is stored, if an event is received.
* @param[inout] bufferSize The size of the buffer. If the function call returns successfully, bufferSize will contain the actual size of the
* requirement ID (if it fits in the buffer) or the required buffer size (if it doesn't not fit in the buffer).
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
* @retval MUI_NO_EVENT Returned if there is no event for the specified application available.
*
* If there is no event for the specified application available, the function returns immediately. Use this function instead of
* MUI_waitEvent() in an application that should not block when no user input is available, e.g., in the game loop of a game. After the data
* contained in the event has been extracted, the interaction event should be destroyed using MUI_destroyEvent().
*/
DLLEXPORT MUI_ERRCODE MUI_pollEvent(MUI_HEVENT* eventHandle, const char* applicationID, char* requirementBuffer, uint16_t* bufferSize);

/**
* @brief Wait for an interaction events destined for the specified application.
* @param[out] eventHandle A handle to the interaction event, if an event is recieved.
* @param[in] applicationID The application ID for which an event should be received.
* @param[in] timeout The maximal amout of time (in milliseconds) to wait.
* @param[out] requirementBuffer A buffer in which the requirement ID of the received event is stored, if an event is received.
* @param[inout] bufferSize The size of the buffer. If the function call returns successfully, bufferSize will contain the actual size of the
* requirement ID (if it fits in the buffer) or the required buffer size (if it doesn't not fit in the buffer).
* @retval MUI_SUCCESS Returned if no error occured.
* @retval MUI_FAILURE Returned if an error occured. Use MUI_getError() to retrieve the most recent error.
* @retval MUI_NO_EVENT Returned if no event for the specified application was available before the timeout occured.
*
* Use this function instead of MUI_pollEvent() in an application that only needs to perform an action after a user input has occured. After
* the data contained in the event has been extracted, the interaction event should be destroyed using MUI_destroyEvent().
*/
DLLEXPORT MUI_ERRCODE MUI_waitEvent(MUI_HEVENT* eventHandle, const char* applicationID, uint16_t timeout, char* requirementBuffer,
                                    uint16_t* bufferSize);

/** @} */ // end of sendreceive_group


/******************************************************
        Error Handling
******************************************************/
/** @defgroup error_group Error Handling
*  Functions relating to error handling.
*  @{
*/

/**
* @brief Returns the error text of the most recent error.
* @returns The error text.
*/
DLLEXPORT const char* MUI_getError();

/** @} */ // end of error_group

#ifdef __cplusplus
}
#endif