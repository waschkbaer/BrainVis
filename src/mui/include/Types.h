/****************************************************************
* Copyright (C) 2016 Andrey Krekhov, David McCann
*
* The content of this file may not be copied and/or distributed
* without the expressed permission of the copyright owner.
*
****************************************************************/

/**
* @file Types.h
* @brief This header file contains type definitions for the public interface of the MorphableUI Gate.
* @date 02/16/2016
* @version 1.0-rc1
*/

#pragma once

typedef unsigned int MUI_HEVENT; /**< Handle to an interaction event. */
typedef int MUI_ERRCODE;         /**< Error code. */

#define MUI_FAILURE 0     /**< Indicates an unsuccessful function call. */
#define MUI_SUCCESS 1     /**< Indicates a successful function call. */
#define MUI_NO_EVENT 2    /**< Indicates that no event was received. */
#define MUI_NO_RECEIVER 3 /**< Indicates the no event was sent because no receiver was found. */

/**
* Identifies an IP endpoint.
*/
struct MUI_Endpoint {
    const char* host; /**< IP or hostname. */
    const char* port; /**< Port number. */
};

/**
* The gate's global configuration.
*/
struct MUI_GateConfiguration {
    MUI_Endpoint serverEndpoint; /**< Endpoint of the server */
    const char* serverLocalPort; /**< Port to which the server connects. Set to 'auto' to let the gate choose a port. */
    const char* gateLocalPort;   /**< Port to which other gates connect. Set to 'auto' to let the gate choose a port. */
    const char* password;        /**< Password to connect to the server. Leave empty if no password is required. */
};

/**
 * Interaction event value types.
 */
enum MUI_EventValueType {
    MUI_INT32, /**< 32-bit signed integer. */
    MUI_BOOL,  /**< Boolean value. */
    MUI_FLOAT, /**< 32-bit floating point number. */
    MUI_STRING /**< String (CAUTION: Currently, no specific encoding is enforced upon sender and receiver. The receiver will receive the
                  string as it is sent by the sender). */
};