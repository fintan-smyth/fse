# FSE

F(ile) S(ystem) E(xplorer) - A simple terminal file manager written in C.

This is just a fun little project I have been doing to test my skills in C, not to be taken too seriously. Heavy inspiration is taken from other terminal file mangers such as lf.

## Features

Currently there is only basic fuctionality. You can navigate through the filesystem and copy, cut, and paste files and directories. Files and empty directories can also be deleted. A simple text file preview is included.

### Binds
```
q	- quit

j	- Select next

k	- Select previous

h	- Go to parent directory

l	- Open

y	- Copy selected

x	- Copy selected

p	- Paste selected

D	- Delete selected

e	- Open selected in editor

E	- Open current directory in editor

:	- Execute shell command

H	- Toggle hidden file visibility

P	- Toggle text file preview

R	- Reload directory

?	- Display this helpful page!
```
## Building

Simply clone this repository and run `make`. I think the only dependencies should be GCC and coreutils.
