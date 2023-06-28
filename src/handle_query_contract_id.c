#include "ens_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    switch (context->selectorIndex) {
        case COMMIT:
            strlcpy(msg->version, "Commit", msg->versionLength);
            break;
        case REGISTER:
            strlcpy(msg->version, "Register", msg->versionLength);
            break;
        case REGISTER_WITH_CONFIG:
            strlcpy(msg->version, "RegisterWithConfig", msg->versionLength);
            break;
        case RENEW:
            strlcpy(msg->version, "Renew", msg->versionLength);
            break;
        case SET_NAME:
            strlcpy(msg->version, "SetName", msg->versionLength);
            break;
        case RENEW_ALL:
            strlcpy(msg->version, "RenewAll", msg->versionLength);
            break;
        case PROVE_AND_CLAIM:
            strlcpy(msg->version, "ProveAndClaim", msg->versionLength);
            break;
        case PROVE_AND_CLAIM_RESOLVER:
            strlcpy(msg->version, "ProveAndClaimWithResolver", msg->versionLength);
            break;
        case SET_OWNER:
            strlcpy(msg->version, "SetOwner", msg->versionLength);
            break;
        case SET_RESOLVER:
            strlcpy(msg->version, "SetResolver", msg->versionLength);
            break;
        case SET_SUBNODE:
            strlcpy(msg->version, "SetSubnode", msg->versionLength);
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
