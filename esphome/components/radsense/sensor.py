import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    ICON_RADIOACTIVE,
    UNIT_EMPTY,
    ICON_CHIP,
    DEVICE_CLASS_FIRMWARE,
    DEVICE_CLASS_EMPTY,
    ENTITY_CATEGORY_DIAGNOSTIC
)

from .const import (
    UNIT_MICROROENTGEN_PER_HOUR,
    UNIT_COUNT_PER_MINUTE,
    CONF_DYNAMIC_INTENSITY,
    CONF_STATIC_INTENSITY,
    CONF_COUNTS_PER_MINUTE,
    CONF_SENSITIVITY,
    CONF_FIRMWARE_VERSION
)

DEPENDENCIES = ["i2c"]

radsense_ns = cg.esphome_ns.namespace("radsense")

RadSenseComponent = radsense_ns.class_(
    "RadSenseComponent", cg.PollingComponent, i2c.I2CDevice
)

intensity_schema = sensor.sensor_schema(
    unit_of_measurement=UNIT_MICROROENTGEN_PER_HOUR,
    icon=ICON_RADIOACTIVE,
    accuracy_decimals=1,
    state_class=STATE_CLASS_MEASUREMENT,
    device_class=DEVICE_CLASS_EMPTY
)
counts_per_minute_schema = sensor.sensor_schema(
    unit_of_measurement=UNIT_COUNT_PER_MINUTE,
    icon=ICON_RADIOACTIVE,
    accuracy_decimals=1,
    state_class=STATE_CLASS_MEASUREMENT,
    device_class=DEVICE_CLASS_EMPTY
)
firmware_version_schema = sensor.sensor_schema(
    unit_of_measurement=UNIT_EMPTY,
    icon=ICON_CHIP,
    accuracy_decimals=0,
    state_class=STATE_CLASS_MEASUREMENT,
    device_class=DEVICE_CLASS_FIRMWARE,
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(RadSenseComponent),
            cv.Optional(CONF_ADDRESS): cv.i2c_address,
            cv.Optional(CONF_SENSITIVITY, default = 105): cv.uint16_t,
            cv.Optional(CONF_DYNAMIC_INTENSITY): intensity_schema,
            cv.Optional(CONF_STATIC_INTENSITY): intensity_schema,
            cv.Optional(CONF_COUNTS_PER_MINUTE): counts_per_minute_schema,
            cv.Optional(CONF_FIRMWARE_VERSION): firmware_version_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x66))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_DYNAMIC_INTENSITY in config:
        sens = await sensor.new_sensor(config[CONF_DYNAMIC_INTENSITY])
        cg.add(var.set_dynamic_intensity_sensor(sens))
    if CONF_STATIC_INTENSITY in config:
        sens = await sensor.new_sensor(config[CONF_STATIC_INTENSITY])
        cg.add(var.set_static_intensity_sensor(sens))
    if CONF_COUNTS_PER_MINUTE in config:
        sens = await sensor.new_sensor(config[CONF_COUNTS_PER_MINUTE])
        cg.add(var.set_counts_per_minute_sensor(sens))
    if CONF_FIRMWARE_VERSION in config:
        sens = await sensor.new_sensor(config[CONF_FIRMWARE_VERSION])
        cg.add(var.set_firmware_version_sensor(sens))

    cg.add(var.set_sensitivity(config[CONF_SENSITIVITY]))
