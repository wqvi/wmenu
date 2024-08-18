# What is this patch?

I needed a menu that had non blocking stdin. Rofi is the only menu that does this, but Gentoo doesn't have rofi-wayland in their main repositories just yet. So I found a simple menu tool and patched it. It has a new flag -z which toggles outputting of the index of input. This patch adds stdin to the poll array in wayland.c and checks if stdin is ready to be read in the main loop. I tried using epoll but found that to be cumbersome and didn't really work in my experience. This program will segfault, I will work on that when it inconviences me.

# wmenu

wmenu is an efficient dynamic menu for Sway and wlroots based Wayland
compositors. It provides a Wayland-native dmenu replacement which maintains the
look and feel of dmenu.

## Installation

Dependencies:

- cairo
- pango
- wayland
- xkbcommon
- scdoc (optional)

```
$ meson build
$ ninja -C build
# ninja -C build install
```

## Usage

See wmenu(1)

To use wmenu with Sway, you can add the following to your configuration file:

```
set $menu wmenu-run
bindsym $mod+d exec $menu
```
