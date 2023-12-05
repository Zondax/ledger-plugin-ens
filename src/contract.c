#include "ens_plugin.h"

// ETHRegistrar
static const uint32_t COMMIT_SELECTOR = 0xf14fcbc8;
static const uint32_t REGISTER_SELECTOR = 0x85f6d155;
static const uint32_t REGISTER_WITH_CONFIG_SELECTOR = 0xf7a16963;
static const uint32_t RENEW_SELECTOR = 0xacf1a841;
// ReverseRegistrar
static const uint32_t SET_NAME_SELECTOR = 0xc47f0027;

// Bulk Renewal
static const uint32_t RENEW_ALL_SELECTOR = 0xe8d6dbb4;

// DNSRegistrar
static const uint32_t PROVE_AND_CLAIM_SELECTOR = 0x8bbedf75;
static const uint32_t PROVE_AND_CLAIM_RESOLVER_SELECTOR = 0x224199c2;

// ENS Registry With Fallback
static const uint32_t SET_OWNER_SELECTOR = 0x5b0fc9c3;
static const uint32_t SET_RESOLVER_SELECTOR = 0x1896f70a;
static const uint32_t SET_SUBNODE_RECORD_SELECTOR = 0x5ef2c7f0;

// Public resolver
static const uint32_t SET_ADDR_SELECTOR = 0x8b95dd71;
static const uint32_t SET_TEXT_SELECTOR = 0x10f13a8c;
static const uint32_t MULTICALL_SELECTOR = 0xac9650d8;
static const uint32_t SET_CONTENT_HASH_SELECTOR = 0x304e6ade;

// Array of all the different ens selectors. Make sure this follows the same order as the
// enum defined in `opensea_plugin.h`
const uint32_t ENS_SELECTORS[NUM_SELECTORS] = {COMMIT_SELECTOR,
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
