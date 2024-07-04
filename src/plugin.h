/*******************************************************************************
 *   Plugin ENS
 *   (c) 2023 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#pragma once

#include <string.h>
#include "eth_plugin_interface.h"

// All possible selectors of your plugin.
// EDIT THIS: Enter your selectors here, in the format X(NAME, value)
// A Xmacro below will create for you:
//     - an enum named selector_t with every NAME
//     - a map named SELECTORS associating each NAME with it's value
#define SELECTORS_LIST(X)                   \
    X(COMMIT, 0xf14fcbc8)                   \
    X(REGISTER, 0x85f6d155)                 \
    X(REGISTER_WITH_CONFIG, 0xf7a16963)     \
    X(RENEW, 0xacf1a841)                    \
    X(SET_NAME, 0xc47f0027)                 \
    X(RENEW_ALL, 0xe8d6dbb4)                \
    X(PROVE_AND_CLAIM, 0x8bbedf75)          \
    X(PROVE_AND_CLAIM_RESOLVER, 0x224199c2) \
    X(SET_OWNER, 0x5b0fc9c3)                \
    X(SET_RESOLVER, 0x1896f70a)             \
    X(SET_SUBNODE, 0x5ef2c7f0)              \
    X(SET_ADDR, 0x8b95dd71)                 \
    X(SET_TEXT, 0x10f13a8c)                 \
    X(MULTICALL, 0xac9650d8)                \
    X(SET_CONTENT_HASH, 0x304e6ade)         \
// Xmacro helpers to define the enum and map
// Do not modify !
#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,

// This enum will be automatically expanded to hold all selector names.
// The value SELECTOR_COUNT can be used to get the number of defined selectors
// Do not modify !
typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

// This array will be automatically expanded to map all selector_t names with the correct value.
// Do not modify !
extern const uint32_t SELECTORS[SELECTOR_COUNT];

// Placeholder variant to be set when parsing is done but data is still being sent.
#define NONE 45

#define HALF_PARAMETER_LENGTH 16

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

// Check if the context structure will fit in the RAM section ETH will prepare for us
// Do not remove!
#ifndef BUILDING_FUZZER
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);
#endif
