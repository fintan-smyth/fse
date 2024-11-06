# FSE

F(ile) S(ystem) E(xplorer) - A simple terminal file manager written in C.

This is just a fun little project I have been doing to test my skills in C, not to be taken too seriously. Heavy inspiration is taken from other terminal file managers, particularly [`lf`](https://github.com/gokcehan/lf).

## Features

Currently there is only basic functionality. Some of the available features are:
- Navigation of the filesystem
- Copying/cutting/pasting files and directories
- Renaming files/directories
- Permanent deletion of files and empty directories
- Search within current directory
- A simple text file preview (scrolling of preview now implemented)
- A (heavily) exa/eza inspired file attribute display
- File-dependent highlighting
- Running executable files

### Binds

|Key|Action|
|:---:|---|
| q | Quit |
| j | Select next |
| k | Select previous |
| h | Go to parent directory |
| l | Open |
| y | Copy selected |
| d | Cut selected |
| p | Paste copy/cut buffer |
| c | Clear copy/cut buffer |
| D | Delete selected |
| r | Rename selected |
| x | Run selected executable |
| e | Open selected in editor |
| E | Open current directory in editor |
| : | Execute shell command |
| / | Search in directory |
| n | Next search result |
| N | Previous Search result |
| g | Select first entry |
| G | Select last entry |
| [ | Scroll up text preview |
| ] | Scroll down text preview |
| H | Toggle hidden file visibility |
| P | Toggle parent directory preview |
| ? | Display these helpful instructions! |

## Building

Simply clone this repository and run `make`, then `make install` should you want it added to your path. 

The only dependencies should be a C compiler and GNU coreutils (or equivalent).
