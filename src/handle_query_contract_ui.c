#include "plugin.h"

static bool set_uint265_with_prefix(const uint8_t *amount,
                                    uint8_t amount_size,
                                    const char *unit,
                                    char *out_buffer,
                                    size_t out_buffer_size) {
    char tmp_buffer[100] = {0};

    if (uint256_to_decimal(amount, amount_size, tmp_buffer, sizeof(tmp_buffer)) == false) {
        return false;
    }

    size_t result_len = strlen(tmp_buffer) + strlen(unit) + 1;  // +1 for the space
    if (out_buffer_size < result_len) {
        return false;
    }

    // Ensure the out_buffer is zeroed
    memset(out_buffer, 0, out_buffer_size);

    // Copy tmp_buffer to out_buffer
    strncpy(out_buffer, tmp_buffer, out_buffer_size - 1);

    // Ensure there is enough space for a space and at least one character from unit
    size_t current_length = strlen(out_buffer);
    if (current_length < out_buffer_size - 1) {
        strncat(out_buffer, " ", out_buffer_size - current_length - 1);
    }

    // Concatenate unit to out_buffer
    strncat(out_buffer, unit, out_buffer_size - strlen(out_buffer) - 1);

    return true;
}

// Set UI for any address screen.
static bool set_address_ui(ethQueryContractUI_t *msg, address_t *value) {
    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        value->value,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        chainid);
}

static bool array_to_hexstr(char *dst,
                            size_t dstLen,
                            const uint8_t *src,
                            uint16_t count,
                            bool addEllipsis) {
    memset(dst, 0, dstLen);
    if (dstLen < (count * 2 + 1)) {
        return false;
    }

    const char hexchars[] = "0123456789abcdef";
    int halfCount = count / 2;  // Calculate the middle point

    for (int i = 0; i < halfCount; i++, src++) {
        *dst++ = hexchars[*src >> 4u];
        *dst++ = hexchars[*src & 0x0Fu];
    }

    if (addEllipsis) {
        // Add "..." in the middle
        *dst++ = '.';
        *dst++ = '.';
        *dst++ = '.';
    }

    for (int i = halfCount; i < count; i++, src++) {
        *dst++ = hexchars[*src >> 4u];
        *dst++ = hexchars[*src & 0x0Fu];
    }

    *dst = 0;  // terminate string

    return true;
}

static bool set_addr_ui(ethQueryContractUI_t *msg, address_t *address, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    return set_address_ui(msg, address);
}

static bool set_bytes32_ui(ethQueryContractUI_t *msg, bytes32_t *array, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    return array_to_hexstr(msg->msg, msg->msgLength, array->value, 32, array->ellipsis);
}

static bool set_bytes_ui(ethQueryContractUI_t *msg,
                         bytes32_t *array,
                         uint16_t src_len,
                         const char *title) {
    strlcpy(msg->title, title, msg->titleLength);

    return array_to_hexstr(msg->msg,
                           msg->msgLength,
                           array->value,
                           (src_len > PARAMETER_LENGTH) ? PARAMETER_LENGTH : src_len,
                           array->ellipsis);
}

static bool set_bytes32_as_int_ui(ethQueryContractUI_t *msg, bytes32_t *array, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    return uint256_to_decimal(array->value, sizeof(array->value), msg->msg, msg->msgLength);
}

static bool set_bytes32_as_int_unit_ui(ethQueryContractUI_t *msg,
                                       bytes32_t *array,
                                       const char *title,
                                       const char *unit) {
    strlcpy(msg->title, title, msg->titleLength);
    return set_uint265_with_prefix(array->value,
                                   sizeof(array->value),
                                   unit,
                                   msg->msg,
                                   msg->msgLength);
}

static bool set_name_ui(ethQueryContractUI_t *msg, name_t *name, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    if (name->ellipsis) {
        char buffer[32];
        strncpy(buffer, (const char *) name->text, 16);
        buffer[16] = '\0';
        strlcat(buffer, "...", sizeof(buffer));
        strlcat(buffer, (const char *) (name->text + 16), sizeof(buffer));
        strncpy(msg->msg, buffer, msg->msgLength - 1);
        msg->msg[msg->msgLength - 1] = '\0';
    } else {
        strncpy(msg->msg, (const char *) name->text, msg->msgLength - 1);
        msg->msg[msg->msgLength - 1] = '\0';
    }
    return true;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    switch (context->selectorIndex) {
        case COMMIT:
            if (msg->screenIndex == 0) {
                ret = set_bytes32_ui(msg, &context->tx.body.commit.commitment, "Commitment");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case REGISTER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_name_ui(msg, &context->tx.body.regist.name, "Name");
                    break;
                case 1:
                    ret = set_addr_ui(msg, &context->tx.body.regist.owner, "Owner");
                    break;
                case 2:
                    ret = set_bytes32_as_int_unit_ui(msg,
                                                     &context->tx.body.regist.duration,
                                                     "Duration",
                                                     "s");
                    break;
                case 3:
                    ret = set_bytes32_ui(msg, &context->tx.body.regist.secret, "Secret");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case REGISTER_WITH_CONFIG:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_name_ui(msg, &context->tx.body.regist_with_config.name, "Name");
                    break;
                case 1:
                    ret = set_addr_ui(msg, &context->tx.body.regist_with_config.owner, "Owner");
                    break;
                case 2:
                    ret =
                        set_bytes32_ui(msg, &context->tx.body.regist_with_config.secret, "Secret");
                    break;
                case 3:
                    ret =
                        set_addr_ui(msg, &context->tx.body.regist_with_config.resolver, "Resolver");
                    break;
                case 4:
                    ret = set_addr_ui(msg, &context->tx.body.regist_with_config.addr, "Addr");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case RENEW:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_name_ui(msg, &context->tx.body.renew.name, "Name");
                    break;
                case 1:
                    ret = set_bytes32_as_int_unit_ui(msg,
                                                     &context->tx.body.renew.duration,
                                                     "Duration",
                                                     "s");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_NAME:
            if (msg->screenIndex == 0) {
                ret = set_name_ui(msg, &context->tx.body.set_name.name, "Name");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case RENEW_ALL:
            if (msg->screenIndex < context->tx.body.renew_all.n_names) {
                ret = set_name_ui(msg, &context->tx.body.renew_all.names[msg->screenIndex], "Name");
            } else if (msg->screenIndex == context->tx.body.renew_all.n_names) {
                ret = set_bytes32_as_int_unit_ui(msg,
                                                 &context->tx.body.renew_all.duration,
                                                 "Duration",
                                                 "s");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }

            break;
        case PROVE_AND_CLAIM:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.prove_claim.name,
                                       context->tx.body.prove_claim.name_len,
                                       "Name");
                    break;
                case 1:
                    ret = set_bytes32_as_int_ui(msg,
                                                &context->tx.body.prove_claim.n_inputs,
                                                "Total Inputs");
                    break;
                case 2:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.prove_claim.proof,
                                       context->tx.body.prove_claim.proof_len,
                                       "Proof");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case PROVE_AND_CLAIM_RESOLVER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.prove_claim_resolver.name,
                                       context->tx.body.prove_claim_resolver.name_len,
                                       "Name");
                    break;
                case 1:
                    ret = set_bytes32_as_int_ui(msg,
                                                &context->tx.body.prove_claim_resolver.n_inputs,
                                                "Total Inputs");
                    break;
                case 2:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.prove_claim_resolver.proof,
                                       context->tx.body.prove_claim_resolver.proof_len,
                                       "Proof");
                    break;
                case 3:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.prove_claim_resolver.resolver,
                                      "Resolver");
                    break;
                case 4:
                    ret = set_addr_ui(msg, &context->tx.body.prove_claim_resolver.addr, "Addr");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_OWNER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_owner.node, "Node");
                    break;
                case 1:
                    ret = set_addr_ui(msg, &context->tx.body.set_owner.owner, "Owner");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_RESOLVER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_resolver.node, "Node");
                    break;
                case 1:
                    ret = set_addr_ui(msg, &context->tx.body.set_resolver.resolver, "Resolver");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_SUBNODE:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_subnode.node, "Node");
                    break;
                case 1:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_subnode.label, "Label");
                    break;
                case 2:
                    ret = set_addr_ui(msg, &context->tx.body.set_subnode.owner, "Owner");
                    break;
                case 3:
                    ret = set_addr_ui(msg, &context->tx.body.set_subnode.resolver, "Resolver");
                    break;
                case 4:
                    ret = set_bytes32_as_int_unit_ui(msg,
                                                     &context->tx.body.set_subnode.ttl,
                                                     "TTL",
                                                     "s");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_ADDR:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_addr.node, "Node");
                    break;
                case 1:
                    ret = set_bytes32_as_int_ui(msg,
                                                &context->tx.body.set_addr.coinType,
                                                "Coin Type");
                    break;
                case 2:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.set_addr.a,
                                       context->tx.body.set_addr.a_len,
                                       "a");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_CONTENT_HASH:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_content_hash.node, "Node");
                    break;
                case 1:
                    ret = set_bytes_ui(msg,
                                       &context->tx.body.set_content_hash.hash,
                                       context->tx.body.set_content_hash.hash_len,
                                       "Hash");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case SET_TEXT:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_bytes32_ui(msg, &context->tx.body.set_text.node, "Node");
                    break;
                case 1:
                    ret = set_name_ui(msg, &context->tx.body.set_text.key, "Key");
                    break;
                case 2:
                    ret = set_name_ui(msg, &context->tx.body.set_text.value, "Value");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case MULTICALL:
            if (msg->screenIndex < context->tx.body.multicall.n_calls) {
                ret = set_bytes_ui(msg,
                                   &context->tx.body.multicall.call[msg->screenIndex],
                                   context->tx.body.multicall.call_len[msg->screenIndex],
                                   "Call");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            ret = false;
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
