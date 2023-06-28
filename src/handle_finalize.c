#include "ens_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // The total number of screen you will need.
    switch (context->selectorIndex) {
        case COMMIT:
        case SET_NAME:
        case PROVE_AND_CLAIM:
            msg->numScreens = 1;
            break;
        case RENEW:
        case RENEW_ALL:
        case PROVE_AND_CLAIM_RESOLVER:
        case SET_OWNER:
        case SET_RESOLVER:
            msg->numScreens = 2;
            break;
        case REGISTER:
            msg->numScreens = 4;
            break;
        case REGISTER_WITH_CONFIG:
        case SET_SUBNODE:
            msg->numScreens = 5;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
