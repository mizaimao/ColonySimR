# ColonySimR
Reimplementation of a ColonySim in C++.

The original repo (written in python 3) can be found at [here](https://github.com/mizaimao/ColonySim).

I could use a refresher on C++ and the original implementation became a bit cluttered as more functions added to it.

![2024-12-28 14 43 26](https://github.com/user-attachments/assets/d9756d1c-cddb-4870-8eb0-2f32faa62f2d)

## Building

Needs a C++20 compiler, [SDL2](https://www.libsdl.org/) and SDL2_ttf. The TOML
parser ([toml11](https://github.com/ToruNiina/toml11)) is vendored in
`src/deps/`, so there is nothing to fetch for it.

macOS (Homebrew):

```sh
brew install sdl2 sdl2_ttf
```

Debian / Ubuntu:

```sh
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

Then:

```sh
make        # builds ./main
make run    # builds and runs
make clean
```

Run it from the repository root: `config.toml` and `Arial.ttf` are both opened
by relative path.

## Controls

| Key       | Action                                  |
| --------- | --------------------------------------- |
| `Space`   | Toggle continuous simulation            |
| `C`       | Advance the simulation by a single tick  |
| `A`       | Add one spore                            |
| `D`       | Quit                                     |

## Configuration

Everything tunable lives in [`config.toml`](config.toml) — window size, colors,
simulation tick rate, and the spore event probabilities. Edit it and restart.

If the file is missing or unparseable the simulation still starts, using the
built-in defaults, and says so on stderr. Unrecognized keys and values of the
wrong type are reported rather than silently ignored.
