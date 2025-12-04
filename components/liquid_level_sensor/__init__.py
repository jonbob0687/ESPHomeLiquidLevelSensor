import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ['i2c']  # Requires I2C bus

liquid_level_sensor_ns = cg.esphome_ns.namespace('liquid_level_sensor')
liquidLevelSensor = liquid_level_sensor_ns.class_('liquidLevelSensor', cg.PollingComponent, cg.sensor.Sensor)

CONFIG_SCHEMA = cg.sensor.sensor_schema(
    liquidLevelSensor,
    unit_of_measurement="%",
    icon="mdi:liquid-percent",
    accuracy_decimals=0
).extend(cv.polling_component_schema('1s'))  # Default poll interval

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], liquidLevelSensor())
    await cg.register_component(var, config)
    await cg.sensor.register_sensor(var, config)