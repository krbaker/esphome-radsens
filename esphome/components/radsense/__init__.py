import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, switch
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
)

from .const import (
    CONF_SENSITIVITY,
)

DEPENDENCIES = ["i2c"]
MULTI_CONF = True

CONF_RADSENSE_ID = "radsense_id"

radsense_ns = cg.esphome_ns.namespace("radsense")

RadSenseComponent = radsense_ns.class_(
    "RadSenseComponent", cg.PollingComponent, i2c.I2CDevice 
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(RadSenseComponent),
            cv.Optional(CONF_ADDRESS): cv.i2c_address,
            cv.Optional(CONF_SENSITIVITY, default = 0): cv.uint16_t,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x66))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_sensitivity(config[CONF_SENSITIVITY]))
