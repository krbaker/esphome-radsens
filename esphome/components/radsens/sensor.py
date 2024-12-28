import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
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
    CONF_FIRMWARE_VERSION
)

from . import CONF_RADSENS_ID, RadSensComponent

DEPENDENCIES = ["radsens"]

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
    device_class=DEVICE_CLASS_EMPTY,
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_RADSENS_ID): cv.use_id(RadSensComponent),
            cv.Optional(CONF_DYNAMIC_INTENSITY): intensity_schema,
            cv.Optional(CONF_STATIC_INTENSITY): intensity_schema,
            cv.Optional(CONF_COUNTS_PER_MINUTE): counts_per_minute_schema,
            cv.Optional(CONF_FIRMWARE_VERSION): firmware_version_schema,
        }
    )
)

async def to_code(config):
    var = await cg.get_variable(config[CONF_RADSENS_ID])
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