#include "ens_plugin.h"

// ETHRegistrar
static const uint8_t COMMIT_SELECTOR[SELECTOR_SIZE] = {0xf1, 0x4f, 0xcb, 0xc8};
static const uint8_t REGISTER_SELECTOR[SELECTOR_SIZE] = {0x85, 0xf6, 0xd1, 0x55};
static const uint8_t REGISTER_WITH_CONFIG_SELECTOR[SELECTOR_SIZE] = {0xf7, 0xa1, 0x69, 0x63};
static const uint8_t RENEW_SELECTOR[SELECTOR_SIZE] = {0xac, 0xf1, 0xa8, 0x41};
// ReverseRegistrar
static const uint8_t SET_NAME_SELECTOR[SELECTOR_SIZE] = {0xc4, 0x7f, 0x00, 0x27};

// Bulk Renewal
static const uint8_t RENEW_ALL_SELECTOR[SELECTOR_SIZE] = {0xe8, 0xd6, 0xdb, 0xb4};

// DNSRegistrar
static const uint8_t PROVE_AND_CLAIM_SELECTOR[SELECTOR_SIZE] = {0x8b, 0xbe, 0xdf, 0x75};
static const uint8_t PROVE_AND_CLAIM_RESOLVER_SELECTOR[SELECTOR_SIZE] = {0x22, 0x41, 0x99, 0xc2};

// ENS Registry With Fallback
static const uint8_t SET_OWNER_SELECTOR[SELECTOR_SIZE] = {0x5b, 0x0f, 0xc9, 0xc3};
static const uint8_t SET_RESOLVER_SELECTOR[SELECTOR_SIZE] = {0x18, 0x96, 0xf7, 0x0a};
static const uint8_t SET_SUBNODE_RECORD_SELECTOR[SELECTOR_SIZE] = {0x5e, 0xf2, 0xc7, 0xf0};

// Public resolver
static const uint8_t SET_ADDR_SELECTOR[SELECTOR_SIZE] = {0x8b, 0x95, 0xdd, 0x71};
static const uint8_t SET_TEXT_SELECTOR[SELECTOR_SIZE] = {0x10, 0xf1, 0x3a, 0x8c};
static const uint8_t MULTICALL_SELECTOR[SELECTOR_SIZE] = {0xac, 0x96, 0x50, 0xd8};
static const uint8_t SET_CONTENT_HASH_SELECTOR[SELECTOR_SIZE] = {0x30, 0x4e, 0x6a, 0xde};

// Array of all the different ens selectors. Make sure this follows the same order as the
// enum defined in `opensea_plugin.h`
const uint8_t *const ENS_SELECTORS[NUM_SELECTORS] = {COMMIT_SELECTOR,
                                                     REGISTER_SELECTOR,
                                                     REGISTER_WITH_CONFIG_SELECTOR,
                                                     RENEW_SELECTOR,
                                                     SET_NAME_SELECTOR,
                                                     RENEW_ALL_SELECTOR,
                                                     PROVE_AND_CLAIM_SELECTOR,
                                                     PROVE_AND_CLAIM_RESOLVER_SELECTOR,
                                                     SET_OWNER_SELECTOR,
                                                     SET_RESOLVER_SELECTOR,
                                                     SET_SUBNODE_RECORD_SELECTOR,
                                                     SET_ADDR_SELECTOR,
                                                     SET_TEXT_SELECTOR,
                                                     MULTICALL_SELECTOR,
                                                     SET_CONTENT_HASH_SELECTOR};
