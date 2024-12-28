from esphome.components import switch
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    ICON_LIGHTBULB,
    ICON_POWER,
    ICON_BATTERY,
    DEVICE_CLASS_SWITCH
)

from ..const import (
    CONF_CONTROL_LED,
    CONF_CONTROL_HIGH_VOLTAGE,
    CONF_CONTROL_LOW_POWER
)

from .. import CONF_RADSENS_ID, RadSensComponent, radsens_ns

LedControl = radsens_ns.class_("LedControl", switch.Switch)
HighVoltageControl = radsens_ns.class_("HighVoltageControl", switch.Switch)
LowPowerControl = radsens_ns.class_("LowPowerControl", switch.Switch)

control_low_power_schema = switch.switch_schema(
    LowPowerControl,
    device_class=DEVICE_CLASS_SWITCH,
    icon=ICON_BATTERY,
    entity_category=ENTITY_CATEGORY_CONFIG,
    default_restore_mode="RESTORE_DEFAULT_OFF"
)
control_high_voltage_schema = switch.switch_schema(
    HighVoltageControl,
    device_class=DEVICE_CLASS_SWITCH,
    icon=ICON_POWER,
    entity_category=ENTITY_CATEGORY_CONFIG,
    default_restore_mode="RESTORE_DEFAULT_ON"
)
control_led_schema = switch.switch_schema(
    LedControl,
    device_class=DEVICE_CLASS_SWITCH,
    icon=ICON_LIGHTBULB,
    entity_category=ENTITY_CATEGORY_CONFIG,
    default_restore_mode="RESTORE_DEFAULT_ON"
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_RADSENS_ID): cv.use_id(RadSensComponent),
            cv.Optional(CONF_CONTROL_LED): control_led_schema,
            cv.Optional(CONF_CONTROL_HIGH_VOLTAGE): control_high_voltage_schema,
            cv.Optional(CONF_CONTROL_LOW_POWER): control_low_power_schema,
        }
    )
)

async def to_code(config):
    var = await cg.get_variable(config[CONF_RADSENS_ID])
    if CONF_CONTROL_LED in config:
        sw = await switch.new_switch(config[CONF_CONTROL_LED])
        await cg.register_parented(sw, config[CONF_RADSENS_ID])
        cg.add(var.set_control_led_switch(sw))
    if CONF_CONTROL_HIGH_VOLTAGE in config:
        sw = await switch.new_switch(config[CONF_CONTROL_HIGH_VOLTAGE])
        await cg.register_parented(sw, config[CONF_RADSENS_ID])
        cg.add(var.set_control_high_voltage_switch(sw))
    if CONF_CONTROL_LOW_POWER in config:
        sw = await switch.new_switch(config[CONF_CONTROL_LOW_POWER])
        await cg.register_parented(sw, config[CONF_RADSENS_ID])
        cg.add(var.set_control_low_power_switch(sw))
