#include "ens_plugin.h"

static void copy_text(uint8_t *dst, uint16_t dst_len, uint16_t max_len, const uint8_t *src) {
    size_t len = MIN(dst_len, max_len);
    memcpy(dst, src, len);
}

static void handle_commit(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case COMMITMENT:
            copy_parameter(context->tx.body.commit.commitment.value,
                           msg->parameter,
                           sizeof(context->tx.body.commit.commitment.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_register(ethPluginProvideParameter_t *msg, context_t *context) {
    bool result = false;
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.regist.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist.owner.value));
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.regist.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist.duration.value));
            context->next_param = SECRET;
            break;
        case SECRET:
            copy_parameter(context->tx.body.regist.secret.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist.secret.value));
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            result = U2BE_from_parameter(msg->parameter, &context->tx.body.regist.name.len);
            if (!result) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            context->next_param = NAME;

            break;
        case NAME:  // wait until reach next field
            copy_text(context->tx.body.regist.name.text,
                      context->tx.body.regist.name.len,
                      PARAMETER_LENGTH,
                      msg->parameter);

            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_register_with_config(ethPluginProvideParameter_t *msg, context_t *context) {
    bool result = false;

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.regist_with_config.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_with_config.owner.value));
            context->offset = 96;
            context->go_to_offset = true;
            context->next_param = SECRET;
            break;
        case SECRET:
            copy_parameter(context->tx.body.regist_with_config.secret.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist_with_config.secret.value));
            context->next_param = RESOLVER;
            break;
        case RESOLVER:
            copy_address(context->tx.body.regist_with_config.resolver.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_with_config.resolver.value));
            context->next_param = ADDR;
            break;
        case ADDR:
            copy_address(context->tx.body.regist_with_config.addr.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_with_config.addr.value));
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            result =
                U2BE_from_parameter(msg->parameter, &context->tx.body.regist_with_config.name.len);
            if (!result) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:  // wait until reach next field
            copy_text(context->tx.body.regist_with_config.name.text,
                      context->tx.body.regist_with_config.name.len,
                      PARAMETER_LENGTH,
                      msg->parameter);

            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_renew(ethPluginProvideParameter_t *msg, context_t *context) {
    bool result = false;
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.renew.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.renew.duration.value));
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            result = U2BE_from_parameter(msg->parameter, &context->tx.body.renew.name.len);
            if (!result) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            context->next_param = NAME;

            break;
        case NAME:  // wait until reach next field
            copy_text(context->tx.body.renew.name.text,
                      context->tx.body.renew.name.len,
                      PARAMETER_LENGTH,
                      msg->parameter);

            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_set_name(ethPluginProvideParameter_t *msg, context_t *context) {
    bool result = false;
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            result = U2BE_from_parameter(msg->parameter, &context->tx.body.set_name.name.len);
            if (!result) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            context->next_param = NAME;

            break;
        case NAME:  // wait until reach next field
            copy_text(context->tx.body.set_name.name.text,
                      context->tx.body.set_name.name.len,
                      PARAMETER_LENGTH,
                      msg->parameter);

            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_renew_all(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.renew_all.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.renew_all.duration.value));
            context->next_param = N_NAME;
            break;
        case N_NAME:
            copy_parameter(context->tx.body.renew_all.n_names.value,
                           msg->parameter,
                           sizeof(context->tx.body.renew_all.n_names.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_prove_claim(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = INPUT_OFFSET;
            break;
        case INPUT_OFFSET:
            context->next_param = INPUT;
            if (!U2BE_from_parameter(msg->parameter, &context->offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->go_to_offset = true;
            break;
        case INPUT:
            copy_parameter(context->tx.body.prove_claim.n_inputs.value,
                           msg->parameter,
                           sizeof(context->tx.body.prove_claim.n_inputs.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_prove_claim_resolver(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = INPUT_OFFSET;
            break;
        case INPUT_OFFSET:
            context->next_param = PROOF_OFFSET;
            break;
        case PROOF_OFFSET:
            context->next_param = RESOLVER;
            break;
        case RESOLVER:
            copy_address(context->tx.body.prove_claim_resolver.resolver.value,
                         msg->parameter,
                         sizeof(context->tx.body.prove_claim_resolver.resolver.value));
            context->next_param = ADDR;
            break;
        case ADDR:
            copy_address(context->tx.body.prove_claim_resolver.addr.value,
                         msg->parameter,
                         sizeof(context->tx.body.prove_claim_resolver.addr.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_owner(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_owner.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_owner.node.value));
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.set_owner.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.set_owner.owner.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_resolver(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_resolver.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_resolver.node.value));
            context->next_param = RESOLVER;
            break;
        case RESOLVER:
            copy_address(context->tx.body.set_resolver.resolver.value,
                         msg->parameter,
                         sizeof(context->tx.body.set_resolver.resolver.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_subnode(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_subnode.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_subnode.node.value));
            context->next_param = LABEL;
            break;
        case LABEL:
            copy_parameter(context->tx.body.set_subnode.label.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_subnode.label.value));
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.set_subnode.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.set_subnode.owner.value));
            context->next_param = RESOLVER;
            break;
        case RESOLVER:
            copy_address(context->tx.body.set_subnode.resolver.value,
                         msg->parameter,
                         sizeof(context->tx.body.set_subnode.resolver.value));
            context->next_param = TTL;
            break;
        case TTL:
            copy_parameter(context->tx.body.set_subnode.ttl.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_subnode.ttl.value));
            context->next_param = NONE;
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
        case COMMIT:
            handle_commit(msg, context);
            break;
        case REGISTER:
            handle_register(msg, context);
            break;
        case REGISTER_WITH_CONFIG:
            handle_register_with_config(msg, context);
            break;
        case RENEW:
            handle_renew(msg, context);
            break;
        case SET_NAME:
            handle_set_name(msg, context);
            break;
        case RENEW_ALL:
            handle_renew_all(msg, context);
            break;
        case PROVE_AND_CLAIM:
            handle_prove_claim(msg, context);
            break;
        case PROVE_AND_CLAIM_RESOLVER:
            handle_prove_claim_resolver(msg, context);
            break;
        case SET_OWNER:
            handle_owner(msg, context);
            break;
        case SET_RESOLVER:
            handle_resolver(msg, context);
            break;
        case SET_SUBNODE:
            handle_subnode(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
