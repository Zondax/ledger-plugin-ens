#include "ens_plugin.h"

static void handle_set_owner(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NODE :  // node
            copy_parameter(context->node,
                           msg->parameter,
                           sizeof(context->node));
            context->next_param = OWNER;
            break;
        case OWNER:  // path
            copy_address(context->owner,
                        sizeof(context->owner),
                        msg->parameter);
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case SET_OWNER_SELECTOR:
            handle_set_owner(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
