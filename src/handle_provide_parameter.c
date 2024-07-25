#include "plugin.h"

uint8_t bytes_missing = 0;

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
            context->tx.body.commit.commitment.ellipsis = false;
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist.offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
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
            context->tx.body.regist.duration.ellipsis = false;
            context->next_param = SECRET;
            break;
        case SECRET:
            copy_parameter(context->tx.body.regist.secret.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist.secret.value));
            context->tx.body.regist.secret.ellipsis = false;
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            if (msg->parameterOffset != context->tx.body.regist.offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.regist.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.regist.name.text,
                          context->tx.body.regist.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.regist.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.regist.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.regist.name.len % PARAMETER_LENGTH;
                context->tx.body.regist.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.regist.name.text,
                          context->tx.body.regist.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.regist.name.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.regist.name.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.regist.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.regist.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.regist.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.regist.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.regist.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.regist.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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

static void handle_register_2(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    uint16_t tmp_offset = 0;
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist_2.offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.regist_2.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_2.owner.value));
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.regist_2.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist_2.duration.value));
            context->tx.body.regist_2.duration.ellipsis = false;
            context->next_param = SECRET;
            break;
        case SECRET:
            copy_parameter(context->tx.body.regist_2.secret.value,
                           msg->parameter,
                           sizeof(context->tx.body.regist_2.secret.value));
            context->tx.body.regist_2.secret.ellipsis = false;
            context->next_param = RESOLVER;
            break;
        case RESOLVER:
            copy_address(context->tx.body.regist_2.resolver.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_2.resolver.value));
            context->next_param = DATA_OFFSET;
            break;
        case DATA_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = REVERSE;
            break;
        case REVERSE:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist_2.reverseRecord)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = CONTROLLED_FUSES;
            break;
        case CONTROLLED_FUSES:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.regist_2.ownerControlledFuses)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            // if (msg->parameterOffset != context->tx.body.regist_2.offset + SELECTOR_SIZE) {
            //     msg->result = ETH_PLUGIN_RESULT_ERROR;
            //     return;
            // }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist_2.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.regist_2.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.regist_2.name.text,
                          context->tx.body.regist_2.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.regist_2.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.regist_2.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.regist_2.name.len % PARAMETER_LENGTH;
                context->tx.body.regist_2.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.regist_2.name.text,
                          context->tx.body.regist_2.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this
                        // one to complete the last 16. then go to last container
                        copy_text(context->tx.body.regist_2.name.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.regist_2.name.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.regist_2.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.regist_2.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.regist_2.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.regist_2.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.regist_2.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.regist_2.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.regist_with_config.offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = OWNER;
            break;
        case OWNER:
            copy_address(context->tx.body.regist_with_config.owner.value,
                         msg->parameter,
                         sizeof(context->tx.body.regist_with_config.owner.value));
            context->offset = msg->parameterOffset + (2 * PARAMETER_LENGTH);
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
            if (msg->parameterOffset !=
                context->tx.body.regist_with_config.offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.regist_with_config.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.regist_with_config.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.regist_with_config.name.text,
                          context->tx.body.regist_with_config.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.regist_with_config.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.regist_with_config.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.regist_with_config.name.len % PARAMETER_LENGTH;
                context->tx.body.regist_with_config.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.regist_with_config.name.text,
                          context->tx.body.regist_with_config.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.regist_with_config.name.text + HALF_PARAMETER_LENGTH,
                            context->tx.body.regist_with_config.name.len - HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.regist_with_config.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.regist_with_config.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.regist_with_config.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.regist_with_config.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.regist_with_config.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.regist_with_config.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.renew.offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.renew.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.renew.duration.value));
            context->tx.body.renew.duration.ellipsis = false;
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            if (msg->parameterOffset != context->tx.body.renew.offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.renew.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            context->next_param = NAME;

            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.renew.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.renew.name.text,
                          context->tx.body.renew.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.renew.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.renew.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.renew.name.len % PARAMETER_LENGTH;
                context->tx.body.renew.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.renew.name.text,
                          context->tx.body.renew.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.renew.name.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.renew.name.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.renew.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.renew.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.renew.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.renew.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.renew.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.renew.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_name.offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            if (msg->parameterOffset != context->tx.body.set_name.offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_name.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.set_name.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.set_name.name.text,
                          context->tx.body.set_name.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.set_name.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                context->tx.body.set_name.name.ellipsis = true;
                containers = context->tx.body.set_name.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.set_name.name.len % PARAMETER_LENGTH;

                // copy first 16 bytes
                copy_text(context->tx.body.set_name.name.text,
                          context->tx.body.set_name.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.set_name.name.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.set_name.name.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.set_name.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.set_name.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.set_name.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.set_name.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.set_name.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.set_name.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    uint16_t tmp_offset = 0;

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            // save temporarily the offset to check on the next parameter we are indeed in the right
            // offset
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.renew_all.offsets_start)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = DURATION;
            break;
        case DURATION:
            copy_parameter(context->tx.body.renew_all.duration.value,
                           msg->parameter,
                           sizeof(context->tx.body.renew_all.duration.value));
            context->tx.body.renew_all.duration.ellipsis = false;
            context->next_param = N_NAME;
            break;
        case N_NAME:
            if (msg->parameterOffset != context->tx.body.renew_all.offsets_start + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.renew_all.n_names) ||
                context->tx.body.renew_all.n_names > 2 || context->tx.body.renew_all.n_names == 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = OFFSETS;
            break;
        case OFFSETS:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            // save offsets start point
            if (context->tx.body.renew_all.id == 0) {
                context->tx.body.renew_all.offsets_start = msg->parameterOffset;
            }
            // save offset and if not last one save next
            context->tx.body.renew_all.offsets[context->tx.body.renew_all.id] = tmp_offset;
            context->tx.body.renew_all.id++;
            context->next_param = OFFSETS;
            if (context->tx.body.renew_all.id == context->tx.body.renew_all.n_names) {
                context->tx.body.renew_all.id = 0;
                context->offset = context->tx.body.renew_all.offsets_start +
                                  context->tx.body.renew_all.offsets[0];
                context->go_to_offset = true;
                context->next_param = NAME_LENGTH;
            }
            break;
        case NAME_LENGTH:
            if (!U2BE_from_parameter(
                    msg->parameter,
                    &context->tx.body.renew_all.names[context->tx.body.renew_all.id].len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;

        case NAME:
            // Name has less then 32
            if (context->tx.body.renew_all.names[context->tx.body.renew_all.id].len <=
                PARAMETER_LENGTH) {
                copy_text(context->tx.body.renew_all.names[context->tx.body.renew_all.id].text,
                          context->tx.body.renew_all.names[context->tx.body.renew_all.id].len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.renew_all.names[context->tx.body.renew_all.id].ellipsis = false;
                if (context->tx.body.renew_all.id == context->tx.body.renew_all.n_names - 1) {
                    context->tx.body.renew_all.id = 0;
                    context->next_param = NONE;
                } else {
                    context->tx.body.renew_all.id++;
                    context->offset =
                        context->tx.body.renew_all.offsets_start +
                        context->tx.body.renew_all.offsets[context->tx.body.renew_all.id];
                    context->go_to_offset = true;
                    context->next_param = NAME_LENGTH;
                }
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.renew_all.names[context->tx.body.renew_all.id].len /
                             PARAMETER_LENGTH;
                bytes_missing =
                    context->tx.body.renew_all.names[context->tx.body.renew_all.id].len %
                    PARAMETER_LENGTH;
                context->tx.body.renew_all.names[context->tx.body.renew_all.id].ellipsis = true;
                // copy first 16 bytes
                copy_text(context->tx.body.renew_all.names[context->tx.body.renew_all.id].text,
                          context->tx.body.renew_all.names[context->tx.body.renew_all.id].len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.renew_all.names[context->tx.body.renew_all.id].text +
                                HALF_PARAMETER_LENGTH,
                            context->tx.body.renew_all.names[context->tx.body.renew_all.id].len -
                                HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.renew_all.names[context->tx.body.renew_all.id].text +
                          HALF_PARAMETER_LENGTH,
                      context->tx.body.renew_all.names[context->tx.body.renew_all.id].len -
                          HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.renew_all.names[context->tx.body.renew_all.id].text +
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.renew_all.names[context->tx.body.renew_all.id].len -
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.renew_all.names[context->tx.body.renew_all.id].text +
                              HALF_PARAMETER_LENGTH,
                          context->tx.body.renew_all.names[context->tx.body.renew_all.id].len -
                              HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }

            if (context->tx.body.renew_all.id == context->tx.body.renew_all.n_names - 1) {
                context->tx.body.renew_all.id = 0;
                context->next_param = NONE;
            } else {
                context->tx.body.renew_all.id++;
                context->offset = context->tx.body.renew_all.offsets_start +
                                  context->tx.body.renew_all.offsets[context->tx.body.renew_all.id];
                context->go_to_offset = true;
                context->next_param = NAME_LENGTH;
            }
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
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.prove_claim.name_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_OFFSET;
            break;
        case INPUT_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.prove_claim.input_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = PROOF_OFFSET;
            break;
        case PROOF_OFFSET:
            context->next_param = NAME_LENGTH;
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.prove_claim.proof_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            break;
        case NAME_LENGTH:
            if (msg->parameterOffset != context->tx.body.prove_claim.name_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.prove_claim.name_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.prove_claim.name_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.prove_claim.name.value,
                          context->tx.body.prove_claim.name_len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.prove_claim.name.ellipsis = false;
                context->next_param = INPUT;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.prove_claim.name_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.prove_claim.name_len % PARAMETER_LENGTH;
                context->tx.body.prove_claim.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.prove_claim.name.value,
                          context->tx.body.prove_claim.name_len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.prove_claim.name.value + HALF_PARAMETER_LENGTH,
                                  context->tx.body.prove_claim.name_len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.prove_claim.name.value + HALF_PARAMETER_LENGTH,
                      context->tx.body.prove_claim.name_len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.prove_claim.name.value + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.prove_claim.name_len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.prove_claim.name.value + HALF_PARAMETER_LENGTH,
                          context->tx.body.prove_claim.name_len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            context->next_param = INPUT;
            break;
        case INPUT:
            if (msg->parameterOffset != context->tx.body.prove_claim.input_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            copy_parameter(context->tx.body.prove_claim.n_inputs.value,
                           msg->parameter,
                           sizeof(context->tx.body.prove_claim.n_inputs.value));
            context->offset = context->tx.body.prove_claim.proof_offset + SELECTOR_SIZE;
            context->go_to_offset = true;
            context->next_param = PROOF_LEN;
            break;
        case PROOF_LEN:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.prove_claim.proof_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = PROOF;
            break;
        case PROOF:
            // Name has less then 32
            if (context->tx.body.prove_claim.proof_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.prove_claim.proof.value,
                          sizeof(context->tx.body.prove_claim.proof.value),
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.prove_claim.proof.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.prove_claim.proof_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.prove_claim.proof_len % PARAMETER_LENGTH;
                context->tx.body.prove_claim.proof.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.prove_claim.proof.value,
                          sizeof(context->tx.body.prove_claim.proof.value),
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.prove_claim.proof.value + HALF_PARAMETER_LENGTH,
                                  sizeof(context->tx.body.prove_claim.proof.value) -
                                      HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PROOF_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PROOF_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = PROOF_2;
                }
            }
            break;
        case PROOF_1:  // second last container
            copy_text(context->tx.body.prove_claim.proof.value + HALF_PARAMETER_LENGTH,
                      sizeof(context->tx.body.prove_claim.proof.value) - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = PROOF_2;
            break;
        case PROOF_2:                                      // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.prove_claim.proof.value + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          sizeof(context->tx.body.prove_claim.proof.value) - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.prove_claim.proof.value + HALF_PARAMETER_LENGTH,
                          sizeof(context->tx.body.prove_claim.proof.value) - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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

uint16_t name_offset = 0;
uint16_t input_offset = 0;
static void handle_prove_claim_resolver(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NAME_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &name_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = INPUT_OFFSET;
            break;
        case INPUT_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &input_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = PROOF_OFFSET;
            break;
        case PROOF_OFFSET:
            context->next_param = RESOLVER;
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.prove_claim_resolver.proof_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
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
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            if (msg->parameterOffset != name_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            name_offset = 0;
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.prove_claim_resolver.name_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.prove_claim_resolver.name_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.prove_claim_resolver.name.value,
                          context->tx.body.prove_claim_resolver.name_len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.prove_claim_resolver.name.ellipsis = false;
                context->next_param = INPUT;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.prove_claim_resolver.name_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.prove_claim_resolver.name_len % PARAMETER_LENGTH;
                context->tx.body.prove_claim_resolver.name.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.prove_claim_resolver.name.value,
                          context->tx.body.prove_claim_resolver.name_len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.prove_claim_resolver.name.value +
                                HALF_PARAMETER_LENGTH,
                            context->tx.body.prove_claim_resolver.name_len - HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.prove_claim_resolver.name.value + HALF_PARAMETER_LENGTH,
                      context->tx.body.prove_claim_resolver.name_len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.prove_claim_resolver.name.value + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.prove_claim_resolver.name_len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.prove_claim_resolver.name.value + HALF_PARAMETER_LENGTH,
                          context->tx.body.prove_claim_resolver.name_len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            context->next_param = INPUT;
            break;
        case INPUT:
            if (msg->parameterOffset != input_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            input_offset = 0;
            copy_parameter(context->tx.body.prove_claim_resolver.n_inputs.value,
                           msg->parameter,
                           sizeof(context->tx.body.prove_claim_resolver.n_inputs.value));
            context->offset = context->tx.body.prove_claim_resolver.proof_offset + SELECTOR_SIZE;
            context->go_to_offset = true;
            context->next_param = PROOF_LEN;
            break;
        case PROOF_LEN:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.prove_claim_resolver.proof_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = PROOF;
            break;
        case PROOF:
            // Name has less then 32
            if (context->tx.body.prove_claim_resolver.proof_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.prove_claim_resolver.proof.value,
                          sizeof(context->tx.body.prove_claim_resolver.proof.value),
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.prove_claim_resolver.proof.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.prove_claim_resolver.proof_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.prove_claim_resolver.proof_len % PARAMETER_LENGTH;
                context->tx.body.prove_claim_resolver.proof.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.prove_claim_resolver.proof.value,
                          sizeof(context->tx.body.prove_claim_resolver.proof.value),
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.prove_claim_resolver.proof.value +
                                      HALF_PARAMETER_LENGTH,
                                  sizeof(context->tx.body.prove_claim_resolver.proof.value) -
                                      HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PROOF_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PROOF_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = PROOF_2;
                }
            }
            break;
        case PROOF_1:  // second last container
            copy_text(
                context->tx.body.prove_claim_resolver.proof.value + HALF_PARAMETER_LENGTH,
                sizeof(context->tx.body.prove_claim_resolver.proof.value) - HALF_PARAMETER_LENGTH,
                HALF_PARAMETER_LENGTH - bytes_missing,
                msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = PROOF_2;
            break;
        case PROOF_2:                                      // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.prove_claim_resolver.proof.value +
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          sizeof(context->tx.body.prove_claim_resolver.proof.value) -
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.prove_claim_resolver.proof.value + HALF_PARAMETER_LENGTH,
                          sizeof(context->tx.body.prove_claim_resolver.proof.value) -
                              HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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
            context->tx.body.set_owner.node.ellipsis = false;
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
            context->tx.body.set_resolver.node.ellipsis = false;
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
            context->tx.body.set_subnode.node.ellipsis = false;
            context->next_param = LABEL;
            break;
        case LABEL:
            copy_parameter(context->tx.body.set_subnode.label.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_subnode.label.value));
            context->tx.body.set_subnode.label.ellipsis = false;
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
            context->tx.body.set_subnode.ttl.ellipsis = false;
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

static void handle_set_addr(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_addr.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_addr.node.value));
            context->tx.body.set_addr.node.ellipsis = false;
            context->next_param = COINTYPE;
            break;
        case COINTYPE:
            copy_parameter(context->tx.body.set_addr.coinType.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_addr.coinType.value));
            context->tx.body.set_addr.coinType.ellipsis = false;
            context->next_param = HASH_OFFSET;
            break;
        case HASH_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_addr.hash_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = HASH_LEN;
            break;
        case HASH_LEN:
            if (msg->parameterOffset != context->tx.body.set_addr.hash_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_addr.a_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = HASH;
            break;
        case HASH:
            // Name has less then 32
            if (context->tx.body.set_addr.a_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.set_addr.a.value,
                          sizeof(context->tx.body.set_addr.a.value),
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.set_addr.a.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.set_content_hash.hash_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.set_content_hash.hash_len % PARAMETER_LENGTH;
                context->tx.body.set_addr.a.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.set_addr.a.value,
                          context->tx.body.set_addr.a_len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.set_addr.a.value + HALF_PARAMETER_LENGTH,
                                  sizeof(context->tx.body.set_addr.a.value) - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = HASH_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = HASH_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = HASH_2;
                }
            }
            break;
        case HASH_1:  // second last container
            copy_text(context->tx.body.set_addr.a.value + HALF_PARAMETER_LENGTH,
                      sizeof(context->tx.body.set_addr.a.value) - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = HASH_2;
            break;
        case HASH_2:
            // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.set_addr.a.value + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          sizeof(context->tx.body.set_addr.a.value) - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.set_addr.a.value + HALF_PARAMETER_LENGTH,
                          sizeof(context->tx.body.set_addr.a.value) - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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

static void handle_set_text(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_text.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_text.node.value));
            context->tx.body.set_text.node.ellipsis = false;
            context->next_param = KEY_OFFSET;
            break;
        case KEY_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_text.key_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = VALUE_OFFSET;
            break;
        case VALUE_OFFSET:
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_text.value_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = KEY_LENGTH;
            break;
        case KEY_LENGTH:
            if (msg->parameterOffset != context->tx.body.set_text.key_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_text.key.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = KEY;
            break;
        case KEY:
            // Name has less then 32
            if (context->tx.body.set_text.key.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.set_text.key.text,
                          context->tx.body.set_text.key.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.set_text.key.ellipsis = false;
                context->next_param = VALUE_LENGTH;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.set_text.key.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.set_text.key.len % PARAMETER_LENGTH;
                context->tx.body.set_text.key.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.set_text.key.text,
                          context->tx.body.set_text.key.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.set_text.key.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.set_text.key.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = KEY_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = KEY_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = KEY_2;
                }
            }
            break;
        case KEY_1:  // second last container
            copy_text(context->tx.body.set_text.key.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.set_text.key.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = KEY_2;
            break;
        case KEY_2:                                        // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.set_text.key.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.set_text.key.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.set_text.key.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.set_text.key.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            context->next_param = VALUE_LENGTH;
            break;
        case VALUE_LENGTH:
            if (msg->parameterOffset != context->tx.body.set_text.value_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_text.value.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = VALUE;
            break;
        case VALUE:
            // Name has less then 32
            if (context->tx.body.set_text.value.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.set_text.value.text,
                          context->tx.body.set_text.value.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.set_text.value.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.set_text.value.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.set_text.value.len % PARAMETER_LENGTH;
                context->tx.body.set_text.value.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.set_text.value.text,
                          context->tx.body.set_text.value.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.set_text.value.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.set_text.value.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = VALUE_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = VALUE_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = VALUE_2;
                }
            }
            break;
        case VALUE_1:  // second last container
            copy_text(context->tx.body.set_text.value.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.set_text.value.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = VALUE_2;
            break;
        case VALUE_2:                                      // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.set_text.value.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.set_text.value.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.set_text.value.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.set_text.value.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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

static void handle_set_content_hash(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case NODE:
            copy_parameter(context->tx.body.set_content_hash.node.value,
                           msg->parameter,
                           sizeof(context->tx.body.set_content_hash.node.value));
            context->tx.body.set_content_hash.node.ellipsis = false;
            context->next_param = HASH_OFFSET;
            break;
        case HASH_OFFSET:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.set_content_hash.hash_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = HASH_LEN;
            break;
        case HASH_LEN:
            if (msg->parameterOffset !=
                context->tx.body.set_content_hash.hash_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.set_content_hash.hash_len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = HASH;
            break;
        case HASH:
            // Name has less then 32
            if (context->tx.body.set_content_hash.hash_len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.set_content_hash.hash.value,
                          sizeof(context->tx.body.set_content_hash.hash.value),
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.set_content_hash.hash.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.set_content_hash.hash_len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.set_content_hash.hash_len % PARAMETER_LENGTH;
                context->tx.body.set_content_hash.hash.ellipsis = true;

                // copy first 16 bytes
                copy_text(context->tx.body.set_content_hash.hash.value,
                          sizeof(context->tx.body.set_content_hash.hash.value),
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.set_content_hash.hash.value + HALF_PARAMETER_LENGTH,
                            sizeof(context->tx.body.set_content_hash.hash.value) -
                                HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = HASH_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = HASH_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = HASH_2;
                }
            }
            break;
        case HASH_1:  // second last container
            copy_text(context->tx.body.set_content_hash.hash.value + HALF_PARAMETER_LENGTH,
                      sizeof(context->tx.body.set_content_hash.hash.value) - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = HASH_2;
            break;
        case HASH_2:
            // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.set_content_hash.hash.value + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          sizeof(context->tx.body.set_content_hash.hash.value) -
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(
                    context->tx.body.set_content_hash.hash.value + HALF_PARAMETER_LENGTH,
                    sizeof(context->tx.body.set_content_hash.hash.value) - HALF_PARAMETER_LENGTH,
                    HALF_PARAMETER_LENGTH,
                    msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
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

static void handle_multicall(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    uint16_t tmp_offset = 0;

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case CALL_OFFSET:
            // save temporarily the offset to check on the next parameter we are indeed in the right
            // offset
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.multicall.offsets_start)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = N_CALL;
            break;
        case N_CALL:
            if (msg->parameterOffset != context->tx.body.multicall.offsets_start + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.multicall.n_calls) ||
                context->tx.body.multicall.n_calls > 3 || context->tx.body.multicall.n_calls == 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = OFFSETS;
            break;
        case OFFSETS:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            // save offsets start point
            if (context->tx.body.multicall.id == 0) {
                context->tx.body.multicall.offsets_start = msg->parameterOffset;
            }
            // save offset and if not last one save next
            context->tx.body.multicall.offsets[context->tx.body.multicall.id] = tmp_offset;
            context->tx.body.multicall.id++;
            context->next_param = OFFSETS;
            if (context->tx.body.multicall.id == context->tx.body.multicall.n_calls) {
                context->tx.body.multicall.id = 0;
                context->offset = context->tx.body.multicall.offsets_start +
                                  context->tx.body.multicall.offsets[0];
                context->go_to_offset = true;
                context->next_param = CALL_LEN;
            }
            break;
        case CALL_LEN:
            if (!U2BE_from_parameter(
                    msg->parameter,
                    &context->tx.body.multicall.call_len[context->tx.body.multicall.id])) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = CALL;
            break;
        case CALL:
            // Name has less then 32
            if (context->tx.body.multicall.call_len[context->tx.body.multicall.id] <=
                PARAMETER_LENGTH) {
                copy_text(
                    context->tx.body.multicall.call[context->tx.body.multicall.id].value,
                    sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id].value),
                    PARAMETER_LENGTH,
                    msg->parameter);
                context->tx.body.multicall.call[context->tx.body.multicall.id].ellipsis = false;
                if (context->tx.body.multicall.id == context->tx.body.multicall.n_calls - 1) {
                    context->tx.body.multicall.id = 0;
                    context->next_param = NONE;
                } else {
                    context->tx.body.multicall.id++;
                    context->offset =
                        context->tx.body.multicall.offsets_start +
                        context->tx.body.multicall.offsets[context->tx.body.multicall.id];
                    context->go_to_offset = true;
                    context->next_param = CALL_LEN;
                }
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.multicall.call_len[context->tx.body.multicall.id] /
                             PARAMETER_LENGTH;
                bytes_missing = context->tx.body.multicall.call_len[context->tx.body.multicall.id] %
                                PARAMETER_LENGTH;
                context->tx.body.multicall.call[context->tx.body.multicall.id].ellipsis = true;
                // copy first 16 bytes
                copy_text(
                    context->tx.body.multicall.call[context->tx.body.multicall.id].value,
                    sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id].value),
                    HALF_PARAMETER_LENGTH,
                    msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.multicall.call[context->tx.body.multicall.id].value +
                                HALF_PARAMETER_LENGTH,
                            sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id]
                                       .value) -
                                HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = CALL_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = CALL_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = CALL_2;
                }
            }
            break;
        case CALL_1:  // second last container
            copy_text(context->tx.body.multicall.call[context->tx.body.multicall.id].value +
                          HALF_PARAMETER_LENGTH,
                      sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id].value) -
                          HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = CALL_2;
            break;
        case CALL_2:                                       // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(
                    context->tx.body.multicall.call[context->tx.body.multicall.id].value +
                        HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                    sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id].value) -
                        HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                    bytes_missing,
                    msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(
                    context->tx.body.multicall.call[context->tx.body.multicall.id].value +
                        HALF_PARAMETER_LENGTH,
                    sizeof(context->tx.body.multicall.call[context->tx.body.multicall.id].value) -
                        HALF_PARAMETER_LENGTH,
                    HALF_PARAMETER_LENGTH,
                    msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }

            if (context->tx.body.multicall.id == context->tx.body.multicall.n_calls - 1) {
                context->tx.body.multicall.id = 0;
                context->next_param = NONE;
            } else {
                context->tx.body.multicall.id++;
                context->offset = context->tx.body.multicall.offsets_start +
                                  context->tx.body.multicall.offsets[context->tx.body.multicall.id];
                context->go_to_offset = true;
                context->next_param = CALL_LEN;
            }
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
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
        case REGISTER_2:
            handle_register_2(msg, context);
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
        case SET_ADDR:
            handle_set_addr(msg, context);
            break;
        case SET_TEXT:
            handle_set_text(msg, context);
            break;
        case SET_CONTENT_HASH:
            handle_set_content_hash(msg, context);
            break;
        case MULTICALL:
            handle_multicall(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
