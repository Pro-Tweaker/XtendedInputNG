# Need for Speed (Underground & newer) - Xtended Input

A plugin for the PC versions of Black Box's Need for Speed games that brings **native gamepad support** to them: automatic button icons, full rebinding, a restored debug camera and, for Carbon and Underground 2, a right-stick chase camera.

Supported: **Underground, Underground 2, Most Wanted, Carbon, Pro Street, Undercover** and **World** (alpha). 32-bit game executables. Windows 7 builds have been dropped.

> Based on [xan1242/NFS-XtendedInput](https://github.com/xan1242/NFS-XtendedInput) (Most Wanted and newer) and [xan1242/NFSU-XtendedInput](https://github.com/xan1242/NFSU-XtendedInput) (Underground and Underground 2). The upstream projects are no longer maintained; this tree continues from them.

## Features

- Automatic UI and text swapping for button icons: detects whether you are using a keyboard or a controller.
- Xbox (One and 360), PS4/PS3 and Switch icon sets ([AeroWidescreen (Aero_)](https://github.com/AeroWidescreen)), or let the plugin pick from the connected pad.
- Two gamepad backends: **SDL3** (default), which reads Xbox, DualShock/DualSense, Switch Pro and most other pads without a wrapper, or plain **XInput**.
- Every game action is rebindable, per save profile (in-game remapping menu in Pro Street).
- Console parity except rumble: restored analog debug camera, with optional mouse look.
- Circular deadzone with full-range rescaling, so diagonals reach 1.0.
- **Carbon, Underground 2:** right-stick orbit of the in-race chase camera.
- Keyboard and mouse handled over Win32 (no DirectInput), with mouse auto-hiding and an optional OS cursor with a custom skin.
- **Carbon, Underground 2:** pad rumble (Carbon derives it from the car, Underground 2 uses the game's own force feedback).
- DualSense adaptive triggers while racing (all games; Underground and Underground 2 not yet verified).
- A log file (`scripts/XtendedInputNG.log`) for diagnosing problems.

## Quick start

1. Install an **ASI loader** in the game folder (for example [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader), as `dinput8.dll`).
2. Extract the release package into the game folder.
3. Start the game and plug in a controller. Check `scripts/XtendedInputNG.log` if something doesn't work.

## Documentation

Everything else lives in the **[wiki](https://github.com/Pro-Tweaker/XtendedInputNG/wiki)**: installation details, configuration, default controls, per-game event and button names, troubleshooting, feature guides, building from source and known limitations.

## Disclaimer

This is an unofficial fan-made modification, **not affiliated with, endorsed by or sponsored by Electronic Arts or Black Box**. *Need for Speed* and related names are trademarks of Electronic Arts Inc. You need your own copy of the game; no game files are included. The plugin patches the game in memory and is provided **as is, use at your own risk**, see [LICENSE](LICENSE).

**Need for Speed: World** is an online game whose servers have been shut down; support is for offline/community use only. Do not use this plugin (or any ASI mod) on a live online service that forbids game modification, since it can get your account banned. The plugin does not touch networking and has no gameplay advantage features, but the risk is yours.

## Support

If this plugin is useful to you, donations are welcome but never expected. Always double-check the address before sending.

| Network | Address |
| --- | --- |
| Ethereum | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Bitcoin | `bc1qg9cd7xl3qtfng4nfnzp6ql8z2wuh88ukhljttv` |
| Solana | `AeNDJURPXUvGg83WtHzBmB14XxLei8cMihHCkcsE4FC1` |
| Linea | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Base | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Arbitrum | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| BNB Chain | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| OP | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Polygon | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Monad | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Arc | `0x20E44B3441Eaf2D6a56f24913F5c6DB76eC91Dc1` |
| Tron | `TCTBeagQwH1Gjg4khaMVgDZv9CHt6aiGgn` |

## Credits

- [AeroWidescreen (Aero_)](https://github.com/AeroWidescreen): the button icons
- xan1242: [XNFSTPKTool](https://github.com/xan1242/xnfstpktool), used to build the button TPK, and the [string hash](https://github.com/xan1242/NFS_CustomFont/tree/master/Tools/NFS_StringHash) the texture names use
- LINK/2012 and ThirteenAG: [injector](https://github.com/ThirteenAG/injector)
- pulzed: [mINI](https://github.com/pulzed/mINI), the INI parser
- xan1242 and Berkay Yigit: the original plugins ([NFS-XtendedInput](https://github.com/xan1242/NFS-XtendedInput), [NFSU-XtendedInput](https://github.com/xan1242/NFSU-XtendedInput))
- [SDL](https://libsdl.org): gamepad support (zlib license, see `third_party/SDL/LICENSE.txt`)
- Nielk1: [DualSense trigger effect factories](https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db) (MIT), ported for the adaptive triggers
- ThirteenAG's [Widescreen Fix](https://github.com/ThirteenAG/WidescreenFixesPack): reference for the chase camera (Carbon, Underground 2)

## License

Xtended Input NG is released under the [MIT License](LICENSE). It includes or builds on third-party software, each under its own license:

<details>
<summary>Third-party licenses</summary>

#### SDL3 (zlib license)

Shipped as scripts/SDL3.dll. https://libsdl.org

```text
Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
```

#### injector (zlib-style license)

Compiled into the plugin. https://github.com/ThirteenAG/injector

```text
Copyright (C) 2012-2014 LINK/2012 <dma_2012@hotmail.com>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
```

#### mINI (MIT License)

Compiled into the plugin. https://github.com/pulzed/mINI

```text
Copyright (c) 2018 Danijel Durakovic

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

#### DualSense trigger effect factories by Nielk1 (MIT License)

The adaptive trigger effect encoders in the plugin are a C++ port of Nielk1's "Factories for all DualSense trigger effects", published under the MIT License: https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db

```text
Copyright (c) Nielk1 (see the link above for the original notice)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

#### XNFSTPKTool (build tool, not distributed)

Used at package time to build the button icon TPK from the icon textures. It is downloaded from its official release, hash-checked, and is not included in the release packages. https://github.com/xan1242/xnfstpktool

#### Button icons

The Xbox, PlayStation and Switch button icon textures are by AeroWidescreen (Aero_): https://github.com/AeroWidescreen

#### Original plugins

Based on NFS-XtendedInput and NFSU-XtendedInput by xan1242 (Lovro Plese) and Berkay Yigit, MIT licensed (see LICENSE).

</details>
