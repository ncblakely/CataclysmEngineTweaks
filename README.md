# Cataclysm Engine Tweaks

A DLL hook providing several enhancements and quality-of-life features for Homeworld: Cataclysm (or Emergence on GoG), with the main goal of improving compatibility on modern machines.

## Prerequisites

You must own a copy of Homeworld: Emergence on GoG. It is currently unknown whether this is compatible with the original CD release of Homeworld: Cataclysm. Please open an issue if it does not work.

## Installation
To install:

1. Download the latest release and extract to your Homeworld: Emergence game folder.
2. (Optional) Back up rgl.dll and rgld3d.dll if you want to be able to revert to the old Direct3D renderer.
3. Overwrite any existing files if you are upgrading from an older version.

Launch the game with cataclysm_loader.exe instead of cataclysm.exe.

## Features

### New features:
- Allows for increasing the default universe update rate of 16 FPS. This is why ship movement appears choppy in the classic Homeworld games. Supported values are 32 or 64 FPS; higher settings are possible but may cause instability or bugs.
- Unlocks selection of any resolution your system supports in the in-game video options. No need to edit the registry anymore!
- Bundles a [new Direct3D9 renderer](https://github.com/ncblakely/hw_rgl) with better performance, compatibility, and fixes for issues in the original D3D renderer.
- High-detail mode: renders objects at maximum detail and removes active effects cap.
- Configurable autosave interval for SP/MP games.

Edit CataclysmEngineTweaks.ini to change these settings.

![Video](https://github.com/ncblakely/CataclysmEngineTweaks/assets/4529049/a8a97d71-01a8-48cc-8e22-cd200a6ab0b2)

### Bug fixes:
- DirectSound audio is now properly supported by the game engine, providing lower latency.
- Fixed OpenGL renderer being unavailable on newer operating systems (NT4 compatibility fix is no longer needed).
- Fixed current music track not being saved/loaded properly in SP games.
- Fixed F12 key not responding to keypresses after the first one (bound to military parade by default).
- Fixed ship textures randomly reverting to point instead of bilinear filtering, causing them to look pixellated.
- Fixed command ships damaging themselves when firing at certain angles.
- Add .ini option `NoCompiledVertexArrays` to fix graphical corruption in OpenGL for some AMD Radeon users.
- Add .ini option `DisableEqualizer` to fix a rare audio bug causing loud distortions for some users.
- Mission 17 no longer reduces the available texture memory pool for ships, and they should display at full detail.

## Known Issues

* The increased update rate is still in beta and may affect game balance/functionality due to timers running too quickly. Many of these have been fixed already, but please report any issues you find.

## Contributing

Contributions and feature requests are welcome! Please open a pull request, or an issue to request a feature or report a bug. When reporting a bug, attaching a save file is helpful.

Building the solution requires Visual Studio 2022 17.10 or newer and `vcpkg`.

## Legal

This modification was created through reverse engineering and some insights gained from the original Homeworld 1 source code, released by Relic under the (now defunct) RDN license.
The source code for Cataclysm is unfortunately lost and was never released.

This puts the project in a legal gray area, but I've published this since other projects based on the Homeworld 1 code have not been taken down. It is up to the IP owners to object. All ownership of the code belongs to them.

## License

Distributed under the MIT License. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>
