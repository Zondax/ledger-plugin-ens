#include "ens_plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // The total number of screen you will need.
    switch (context->selectorIndex) {
        case COMMIT:
        case SET_NAME:
            msg->numScreens = 1;
            break;
        case RENEW:
        case SET_OWNER:
        case SET_RESOLVER:
        case SET_CONTENT_HASH:
            msg->numScreens = 2;
            break;
        case SET_TEXT:
        case SET_ADDR:
        case PROVE_AND_CLAIM:
            msg->numScreens = 3;
            break;
        case REGISTER:
            msg->numScreens = 4;
            break;
        case REGISTER_WITH_CONFIG:
        case SET_SUBNODE:
        case PROVE_AND_CLAIM_RESOLVER:
            msg->numScreens = 5;
            break;
        case RENEW_ALL:
            msg->numScreens = 1 + context->tx.body.renew_all.n_names;
            break;
        case MULTICALL:
            msg->numScreens = context->tx.body.multicall.n_calls;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
