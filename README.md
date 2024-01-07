# Apple Music Current Song
Unfortunately Apple Music API doesn't have a "get current playing song" method to see what is currently playing.  In an attempt to just play around during break I decided to write an app that would discover what is playing if you have the Apple Music app running on Windows.  Yup, unfortunately this is only supported on Windows as it uses the IUAutomation interfaces heavily to try and discover the song.  Further unfortunate is you need to install the Apple Music Preview app on Windows.  So yeah, not the most useful/generic approach but it was fun to play around with.

## Table of Contents
- Installation
- Compilation
- Contributing

## Installation
Grab AppleMusicFindCurrentSong.exe and AppleMusicPlayer.dll from the latest release, put them in same directory, and launch from command line!

AppleMusicFindCurrentSong [-o|--output-file] <full_path>

## Compilation
- For Windows: Install Visual Studio 2022
- Install cmake (3.28.0 or newer)
- Configure:
cmake --preset windows

- Compile:
cmake --build --preset windows

- If you want to compile debug compile as follows:
cmake --build --preset windowsdebug

## Contributing
I would love contributions, feel free to open an issue or submit a pull request!
