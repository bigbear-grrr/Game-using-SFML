This project was inspired by the classic arcade game galaga, it serves as a fun way to explore game development concepts, including player interaction, dynamic object behavior, and visually appealing graphics.

## features
- Dynamic enemy behaviors (oscillating and zig-zag movement)
- Level progression with increasing difficulty
- Power-ups: health boosts and speed upgrades
- Projectiles + bombs with visual feedback
- Score tracking and animated floating point messages
- Full instruction screen and responsive UI
- Audio feedback

## Technical Details
- **Language:** C++
- **Library:** SFML (Simple and Fast Multimedia Library)
- **IDE:** Visual Studio
- **Assets:** 
  - Textures (.png)
  - Sounds (.wav, .flac)
  - Fonts (.ttf)

![gameSS](https://github.com/user-attachments/assets/0dbad57e-e9d7-4e9c-b8ad-4952e0451966)




**note to self**: 
c++ -> general -> additional include dir. -> $(SolutionDir)external\sfml\include
linker -> general -> additional lib dir. -> $(SolutionDir)external\sfml\lib

linker -> input -> debug -> additional dependencies 
sfml-graphics-d.lib
sfml-window-d.lib
sfml-audio-d.lib
sfml-network-d.lib
sfml-system-d.lib

linker -> input -> release -> additional dependencies 
sfml-graphics.lib
sfml-window.lib
sfml-audio.lib
sfml-network.lib
sfml-system.lib
