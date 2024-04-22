# Configuration

Config json-files are created on first start of the emulator and can be found under "./Config". It's enough to call `emulator.exe --h`.

Versioning is todo, copying old config files may therefore break it.

## controls.json : Gamepad layout

You can change the layout now in _controls.json_, if you want to.

```jsonc
{
  // You don't have to bind all the buttons there.
  // Emulator will show the missing keybind message
  // that you can ignore without any problems.
  // These binds will be used to emulate gamepad actions for the pad with `type` set to "keyboard"
  // Key names can be obtained here: https://wiki.libsdl.org/SDL2/SDL_Scancode
  "keybinds": {
    "circle": "l",
    "cross": "k",
    "dpad_down": "down",
    "dpad_left": "left",
    "dpad_right": "right",
    "dpad_up": "up",
    "l1": "f3",
    "l2": "f5",
    "l3": "space",
    "lx+": "d",
    "lx-": "a",
    "ly+": "s",
    "ly-": "w",
    "options": "f1",
    "r1": "f2",
    "r2": "f6",
    "r3": "home",
    "rx+": "f",
    "rx-": "h",
    "ry+": "g",
    "ry-": "t",
    "square": "j",
    "touchpad": "f4",
    "triangle": "i"
  },
  // This array contains 4 objects with the similar structure.
  // These objects describes the parameters for each pad
  "pads": [
    {
      // Possible values for "type"
      // 1) XInput: xbox/xinput
      // 2) SDL2: gamepad/sdl
      // 3) Keyboard: keyboard/kbd/kb
      "type": "gamepad",
      "deadzones": { // Ignore it for now, not implemented
        "left_stick": {
          "x": 0.0,
          "y": 0.0
        },
        "right_stick": {
          "x": 0.0,
          "y": 0.0
        }
      }
    },
    // More pads here...
  ]
}
```

## general.json

```jsonc
{
  "systemlang": 0, // System langauge, see the list below to get these indexes
  "netEnabled": false, // Wether enable networking features or not. Better to keep it disabled, not ready for the actual use.
  "netMAC": "00:00:00:00:00:00", // Your ethernet adapter's MAC address. Zero-MAC means "first usable device".
  "onlineUsers": 1, // Number of authorized users, 1..4
  "userIndex": 1, // Id of the user that started the game
  "profiles": [ // User profiles, you can change your name there
    {
      "color": "blue",
      "name": "Anon #1"
    },
    // More users here...
  ],
}

```

Game language can be changed with systemlang : *, default is EnglishUS. Game must support it.

  Japanese           = 0 \
  EnglishUS          = 1 \
  French             = 2 \
  Spanish            = 3 \
  German             = 4 \
  Italian            = 5 \
  Dutch              = 6 \
  PortuguesePT       = 7 \
  Russian            = 8 \
  Korean             = 9 \
  ChineseTraditional = 10 \
  ChineseSimplified  = 11 \
  Finnish            = 12 \
  Swedish            = 13 \
  Danish             = 14 \
  Norwegian          = 15 \
  Polish             = 16 \
  PortugueseBR       = 17 \
  EnglishUK          = 18 \
  Turkish            = 19 \
  SpanishLA          = 20 \
  Arabic             = 21 \
  FrenchCA           = 22 \
  Czech              = 23 \
  Hungarian          = 24 \
  Greek              = 25 \
  Romanian           = 26 \
  Thai               = 27 \
  Vietnamese         = 28 \
  Indonesian         = 29

## audio.json

```jsonc
{
  "device": "[default]", // Audio device name, [default] means system default output device
  "volume": 0.05 // Master volume, 0...1
}
```

## graphics.json

> [!WARNING]
> The emulator updates this file every time you gracefully stop the emulation \(by closing the emulator window\)!

```jsonc
{
  "display": 1, // Your display index, starting from 0
  "fullscreen": false, // Wether emulator will run in fullscreen mode or not
  "height": 1080, // The emulator window dimensions, ignored in fullscreen mode
  "width": 1920,
  "xpos": -1, // The emulator window position, -1 means center
  "ypos": -1
}
```

## logging.json

```jsonc
{
  // Log output
  // Possible values:
  // 1) Baical - to Baical server over network
  // 2) FileBin - to local binary file (you should use this format to post compatibility reports)
  // 3) FileTxt - to local text file
  // 4) Syslog - to system log server
  // 5) Console - to console
  // 6) Auto - try send to Baical server if available, otherwise to file
  // 7) Null - drop all incoming data
  "sink": "FileBin",
  "verbosity": 1 // Verbosity level: 0 - Trace, 1 - Debug, 2 - Warnings, 3 - Errors, 4 - Critical
}
```
