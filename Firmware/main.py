print("Starting")

import board
import busio
from board import SCL, SDA

from kmk.modules.layers import Layers
from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.modules.encoder import EncoderHandler
from kmk.extensions.display import Display, TextEntry, ImageEntry
from kmk.extensions.display.ssd1306 import SSD1306 
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.macros import Macros
from kmk.modules.macros import Tap, Press, Release

keyboard = KMKKeyboard()
encoder_handler = EncoderHandler()
layers = Layers()
keyboard.modules = [layers, encoder_handler]
macros = Macros()
keyboard.modules.append(macros)
keyboard.extensions.append(MediaKeys())

i2c_bus = busio.I2C(board.GP6, board.GP7)
driver = SSD1306(
    i2c = i2c_bus,
    device_address=0x3C,
)

display = Display(
    display=driver,
    width=128,
    height=32,
    flip = False,
    brightness=0.8,
)

keyboard.col_pins = (
    board.GP26, 
    board.GP27, 
    board.GP28, 
    board.GP29
    )
keyboard.row_pins = (
    board.GP0, 
    board.GP3, 
    board.GP4
    )
keyboard.diode_orientation = DiodeOrientation.COL2ROW

encoder_handler.pins = (
    (board.GP2, board.GP1, None)
)

extrude = KC.MACRO(
    Press(KC.LSFT),
    Tap(KC.E),
    Release(KC.LSFT)
)

sketch = KC.MACRO(
    Press(KC.LSFT),
    Tap(KC.S),
    Release(KC.LSFT)
)

revolve = KC.MACRO(
    Press(KC.LSFT),
    Tap(KC.W),
    Release(KC.LSFT)
)

fillet = KC.MACRO(
    Press(KC.LSFT),
    Tap(KC.F),
    Release(KC.LSFT)
)

copy = KC.MACRO(
    Press(KC.LCTRL),
    Tap(KC.C),
    Release(KC.LCTRL)
)

paste = KC.MACRO(
    Press(KC.LCTRL),
    Tap(KC.V),
    Release(KC.LCTRL)
)

undo = KC.MACRO(
    Press(KC.LCTRL),
    Tap(KC.Z),
    Release(KC.LCTRL)
)

redo = KC.MACRO(
    Press(KC.LCTRL),
    Tap(KC.Y),
    Release(KC.LCTRL)
)



keyboard.keymap = [
    [KC.MPRV, KC.MPLY, KC.MNXT, KC.MUTE],
    [extrude, sketch, revolve, fillet],
    [undo, paste, undo, redo],
]

encoder_handler.map = [(KC.VOLD, KC.VOLU, None)]

display.entries = [
    ImageEntry("1.bmp", x=0, y=0),
]
keyboard.extensions.append(display)

if __name__ == '__main__':
    keyboard.go()


