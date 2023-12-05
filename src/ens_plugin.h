#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 15

// Name of the plugin.
#define PLUGIN_NAME "ENS"

// Placeholder variant to be set when parsing is done but data is still being sent.
#define NONE 45

#define HALF_PARAMETER_LENGTH 16
// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    COMMIT = 0,
    REGISTER,
    REGISTER_WITH_CONFIG,
    RENEW,
    SET_NAME,
    RENEW_ALL,
    PROVE_AND_CLAIM,
    PROVE_AND_CLAIM_RESOLVER,
    SET_OWNER,
    SET_RESOLVER,
    SET_SUBNODE,
    SET_ADDR,
    SET_TEXT,
    MULTICALL,
    SET_CONTENT_HASH
} selector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    COMMITMENT = 0,
    NAME_OFFSET,
    NAME_LENGTH,
    NAME,
    DURATION,
    SECRET,
    RESOLVER,
    ADDR,
    INPUT,
    INPUT_OFFSET,
    PROOF_OFFSET,
    PROOF,
    PROOF_1,
    PROOF_2,
    PROOF_LEN,
    NODE,
    OWNER,
    LABEL,
    TTL,
    N_NAME,
    COINTYPE,
    NAME_OFFSET_1,
    NAME_OFFSET_2,
    VALUE,
    KEY,
    VALUE_OFFSET,
    KEY_OFFSET,
    VALUE_LENGTH,
    KEY_LENGTH,
    VALUE_1,
    KEY_1,
    VALUE_2,
    KEY_2,
    HASH,
    HASH_OFFSET,
    HASH_1,
    HASH_2,
    HASH_LEN,
    CALL,
    CALL_OFFSET,
    CALL_1,
    CALL_2,
    CALL_LEN,
    N_CALL,
    OFFSETS,
    UNEXPECTED_PARAMETER,
} parameter;

extern const uint8_t *const ENS_SELECTORS[NUM_SELECTORS];

typedef struct {
    uint8_t value[ADDRESS_LENGTH];
} address_t;

typedef struct {
    uint16_t len;
    uint8_t text[PARAMETER_LENGTH + 1];
    bool ellipsis;
} name_t;

typedef struct {
    uint8_t value[INT256_LENGTH];
    bool ellipsis;
} bytes32_t;

typedef struct {
    union {
        struct {
            bytes32_t commitment;
        } commit;

        struct {
            name_t name;
            address_t owner;
            bytes32_t duration;
            bytes32_t secret;
            uint16_t offset;
        } regist;

        struct {
            name_t name;         // 32
            address_t owner;     // 20
            bytes32_t secret;    // 32
            address_t resolver;  // 20
            address_t addr;      // 20
            uint16_t offset;     // 2
        } regist_with_config;

        struct {
            name_t name;
            bytes32_t duration;
            uint16_t offset;
        } renew;

        struct {
            name_t name;
            uint16_t offset;
        } set_name;

        struct {
            bytes32_t duration;
            uint16_t n_names;
            name_t names[2];
            uint8_t id;
            uint16_t offsets[2];
            uint16_t offsets_start;
        } renew_all;

        struct {
            bytes32_t name;         // 32
            uint16_t name_len;      // 2
            bytes32_t proof;        // 32
            uint16_t proof_offset;  // 2
            uint16_t proof_len;     // 2
            bytes32_t n_inputs;     // 32
            uint16_t name_offset;
            uint16_t input_offset;
        } prove_claim;

        struct {
            bytes32_t name;         // 32
            uint16_t name_len;      // 2
            bytes32_t proof;        // 32
            uint16_t proof_offset;  // 2
            uint16_t proof_len;     // 2
            bytes32_t n_inputs;     // 32
            address_t resolver;     // 20
            address_t addr;         // 20
        } prove_claim_resolver;

        struct {
            bytes32_t node;
            address_t owner;
            const uint8_t *ptr;
        } set_owner;

        struct {
            bytes32_t node;
            bytes32_t label;
            address_t owner;
            address_t resolver;
            bytes32_t ttl;
        } set_subnode;

        struct {
            bytes32_t node;
            address_t resolver;
        } set_resolver;

        struct {
            bytes32_t node;
            bytes32_t coinType;
            uint16_t a_len;
            bytes32_t a;
            uint16_t hash_offset;
        } set_addr;

        struct {
            bytes32_t node;
            name_t key;
            name_t value;
            uint16_t value_offset;
            uint16_t key_offset;
        } set_text;

        struct {
            bytes32_t node;
            uint16_t hash_len;
            bytes32_t hash;
            uint16_t hash_offset;
        } set_content_hash;

        struct {
            uint16_t n_calls;
            uint16_t call_len[3];
            bytes32_t call[3];
            uint8_t id;
            uint16_t offsets[3];
            uint16_t offsets_start;
        } multicall;
    } body;
} ens_tx_t;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;

    // Tx related context
    ens_tx_t tx;
} context_t;

ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);
