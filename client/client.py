import os
import sys
import serial
import logging
from datetime import datetime

# Main method and loop
def main():
  init_logger()

  serial_client = open_serial_client()

  while True:
    message = get_next_message(serial_client)
    if message != None:
      logging.info(message)

# Initializes the logger
def init_logger():
  log_level_str = os.environ.get('LOG_LEVEL') or 'INFO'
  log_level = getattr(logging, log_level_str)
  logging.basicConfig(level=log_level)
  logging.info('Wifi Analyzer logging started')

# Opens the serial device client
def open_serial_client():
  baud_rate_str = os.environ.get('BAUD_RATE') or '115200'
  baud_rate = int(baud_rate_str)
  device = os.environ.get('DEVICE') or "/dev/tty.SLAB_USBtoUART"

  ser = serial.Serial()
  ser.baudrate = baud_rate
  ser.bytesize=serial.SEVENBITS
  ser.parity=serial.PARITY_EVEN
  ser.stopbits=serial.STOPBITS_ONE
  ser.xonxoff=0
  ser.rtscts=0
  ser.timeout=20
  ser.port=device

  try:
    ser.open()
  except err:
    logging.critical(f'Cannot open serial device: {err}')
    sys.exit(f'Cannot open serial device: {err}')

  return ser

# Reads the next message from the serial device
def get_next_message(ser):
  try:
    return ser.readline()
  except error:
    logging.error(f'Cannot read from serial device: {error}')
    return None

main()