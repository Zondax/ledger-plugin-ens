#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 3

// Name of the plugin.
#define PLUGIN_NAME "ENS"

#define NONE                  13  // Placeholder variant to be set when parsing is done but data is still being sent.


// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    SET_OWNER_SELECTOR = 0,
    SET_SUBNODE_RECORD_SELECTOR,
    SET_RESOLVER_SELECTOR,
} selector_t;

// Enumeration used to parse the smart contract data.
// EDIT THIS: Adapt the parameter names here.
typedef enum {
    NODE = 0,
    OWNER,
} parameter;

extern const uint32_t ENS_SELECTORS[NUM_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For display.
    uint8_t node[INT256_LENGTH];
    uint8_t owner[ADDRESS_LENGTH];


    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);
