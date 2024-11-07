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
- Editable config

### Default Binds

|Default Key|Keyword|Action|
|:---:|:---:|---|
| q | QUIT | Quit |
| j | SELECT_NEXT |  Select next |
| k | SELECT_PREV |  Select previous |
| h | UPDIR |  Go to parent directory |
| l | OPEN |  Open |
| y | YANK |  Copy selected |
| d | CUT |  Cut selected |
| p | PASTE |  Paste copy/cut buffer |
| c | CLEAR_BUF |  Clear copy/cut buffer |
| D | DELETE |  Delete selected |
| r | RENAME |  Rename selected |
| x | EXEC_FILE |  Run selected executable |
| e | EDIT_FILE |  Open selected in editor |
| E | EDIT_DIR |  Open current directory in editor |
| : | EXEC_SHELL |  Execute shell command |
| / | SEARCH_DIR |  Search in directory |
| n | SEARCH_NEXT |  Next search result |
| N | SEARCH_PREV |  Previous Search result |
| g | GO_FIRST |  Select first entry |
| G | GO_LAST |  Select last entry |
| [ | PREV_UP |  Scroll up text preview |
| ] | PREV_DOWN |  Scroll down text preview |
| H | TOGGLE_HIDDEN |  Toggle hidden file visibility |
| S | TOGGLE_SORT |  Toggle between alphabetical/filesize sorting |
| P | TOGGLE_PARENT |  Toggle parent directory preview |
| ? | HELP |  Display these helpful instructions! |

## Config

The program binds can now be edited by adjusting the config file, which must be located at `~/.config/fse/`.
A config file containing the default binds is included in this repository, and will be copied to its correct location on installation.
Any unset options will use their default binds.
The format for the config file is:
	`{KEYWORD} = {key}`
where {KEYWORD} is a keyword specified in the table of binds above, and {KEY} is any printable character.
Elements of a line in the config file can be separated by an indeterminate amount of non-newline whitespace.

## Building

Simply clone this repository and run `make`, then `make install` should you want it added to your path. 

The only dependencies should be a C compiler and GNU coreutils (or equivalent).
