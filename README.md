# gxsradio

Version 0.1b, March 2020

## What is this

`gxsradio` is a GTK3-based, graphical Internet radio stream manager and
selector for use with the `xine-server` network-controllable stream player.
With `xine-server` it forms an Internet radio player with multiple points of
control.

`xine-server` is available at https://github.com/kevinboone/xine-server.

`gxsradio` is one of a set of programs designed to work together to provide a
complete multi-room audio system using embedded Linux systems like Raspberry
Pi.  It has been designed to build and run on a fairly minimal Raspberry Pi
installation, just as `xine-server` has.  However, being GTK-based it needs a
full set of GTK libraries and headers on the Pi to build successfully, not to
mention an X server and at least a minimal window manager to run it. Although
it is GTK-based it is designed not to need any Gnome infrastructure and will
run on X using a lightweight window manager like Matchbox. 

In fact, I designed the very simple user interface of `gxsradio` for use on a
Rasperry Pi with 5-inch or 7-inch touchscreen, and a minimal Linux installation
with a read-only root filesystem.  The build does not assume any platform
package management is in place, which reduces the need for a lot of desktop
build stuff on the embedded system -- but it does mean that the Makefile needs
to be provided with hard-coded references to all the required libraries. 

In a very minimal Linux installation be aware that, in addition to
the GTK libraries, GTK needs at runtime at least the contents
of `/usr/share/mime` and a compiled GLib schemas file, the location of
which will vary from system to system. The system does not need a
desktop, `systemd`, `udev`, or DBus -- but you should expect lots of console
error messages without DBus. It's difficult even to get an X server
to run without trying to talk to DBus these days.

A console-based Internet radio player, called `xsradio`, is part of
the `xine-server` project itself. Both `xsradio` and `gxsradio` use
the same station list files, and have similar functionality -- they
even look similar, by design. However, `xsradio` will run in a
Linux kernel console, so it will work in an even more minimal Linux
installation the `gxsradio` requires.

## Internet radio, and its limitations

There is no reliable way to know for sure how many Internet radio
stations there are -- tens of thousands at least. 
My experience is that, even of those stations which I have
known to work in the past, at least half do not work at any particular
time. In the various lists of stations to be found on Web sites,
probably 80% of the stations do not work, at least from my region. 

Commercial vendors of Internet radio devices maintain curated
lists of stations -- this is a large part of what you pay for when 
you buy a commercial device. To be best of my knowledge, these
station lists are proprietary, and there is no legal way to distribute
them. In the public domain, I think the next best thing to a
commercial station list is the community-maintained
Community Radio Station Index (CRSI) website
at `radio-browser.info`. This list is reasonably reliable, to the
extent that the station URLs are at least checked to ensure that
a connection can be made. Nevertheless, being community-supported,
the data is of variable quality, and you can't expect every
station to work. 

`gxsradio` is supplied with a few sample station files, and
the Perl script that extracts them from `radio-browser.info` --
see below for how to use this.

There are various way in which `gxsradio` -- or, more accurately, xine-server
-- can fail to play a station.  First, the URI might not resolve at all -- this
is the simplest case, because the program will display an error message
immediately.  Second, the URI might resolve, but no connection be made. Again,
this is reported immediately.  Third, a connection might be established, and
then be closed immediately from the server end. This might happen if the server
is too busy, or doesn't like the player for some reason.  This appears in
`xsradio` as if, essentially, nothing happened -- the user interface will
continue to display "stopped". Fourth, the station might start to deliver
audio, but continually be interrupted by buffering. This behaviour results in
`gxsradio` showing the status as 'buffering' either frequently, or for extended
periods. In general, if a station doesn't start delivering audio after a few
seconds, it probably isn't going to. However, I do know of a few stations that
have a 30-second buffering period, and then play perfectly well.

The point I wish to make here is that, although `gxsradio` or `xine-server`
might have bugs, problems in the use of this software are at least as likely to
be due to the unreliable nature of Internet radio stations themselves.


## Pre-requisites

To run `gxsradio` at least one working installation of `xine-server` is
required. This can be on the same host as `gxsradio`, or on some other 
host reachable over a network. 

On the `gxsradio` host, a full set of GTK+ 3.0 
libraries is required. On any desktop-like system, even Raspbian, all this
stuff is likely to be installed by default. However, on an embedded 
system, there's likely to be no shortcut to examining the Makefile and
working out what specific libraries are required. I've tried to minimize
the dependencies by, for example, building icon images directly into
the binary, but GTK+ does have a lot of unavoidable dependencies.

To build, you will need development headers for all the GTK components.
On a desktop system with a package manager, you can probably get most
of what is required by installing the 
`libgtk-3-dev` and `libgdk-pixbuf2.0-dev` packages.  

`gsxradio` is designed to run under Linux, and be built with a modern
version of GCC. I do not know whether other systems, or other compilers,
will work -- but I suspect not.

## Build and install 

In principle, it's just the usual:

    $ make
    $ sudo make install

However, some fiddling with the Makefile is likely to be required, because
it does not assume any kind of package manager is available, and
many directory locations are hard-coded.

    $ sudo make install-desktop

Will install a `.desktop` file and icon, and update the MIME database,
for desktop systems. Whether this works or not is highly system-dependent.
Of course, it's unlikely to be remotely useful on an embedded system with
no desktop to speak of.

In principle, no build tools are required other than the compiler -- in
particular, the GTK user interface is specified entirely in C code, so no
UI-related build tools are required.

## Operation

`gxsradio` requires a list of stations. The format of the stations file,
and a way to obtain one, are described below.

The first time `gxsradio` is started, it will prompt for the host and
port of the `xine-server` audio server, unless these have been specified
on the command line. Like most settings, these values will be saved
in the RC file (see below) for next time. 

Hopefully, the actual operation of the program is self-explanatory -- it's
mostly just a matter of double-clicking a station from the list. The
list can be sorted in various ways using the column headers, and edited
if necessary.

## RC file

Per-user configuration values are stored in `$HOME/.gsxradio.rc`.

## Command-line 

The basic command line is

    $ gxsradio {options} {stations file}

The stations file (the format of which is described below) can
have any name, but the user interface will default to 
selecting files ending in `.gxsradio`. 

## Command-line options

Note that all command-line settings, if valid, will be written to the
RC file for later use.

`-c,--css {filename}`

Specify the name of a CSS file for theming. This option is intended
for use in embedded systems, where the usual GTK theming apparatus
may not be available. This setting is not saved in the RC file -- it is
intended for use in situations where RC files could not be saved.

See the 'Styling' section for more details

`-d,--fixed-dir {directory}

Limit the user interface file selection dialog boxes to the specific
directory, and only files ending in '.gxsradio'. This setting applies
to both file-open and file-save operations although, if `--readonly`
is set, no file-save operations will be allowed, regardless of
the user interface.

This option is intended to use in embedded or kiosk-type applications,
where the user will have access only to a set of pre-defined 
station files. However, not that this is a convenience feature, not
a security feature -- a user might be able to defeat the directory
limit by a cunning use of ".." or similar in the filename.

This setting makes `gxsradio` use internal, simplified dialog boxes,
rather than the GTK platform dialogs. On an embeded system that might be
a useful feature in its own right, because the stock GTK3 dialog
boxes are too large to fit on a small screen.

`-f,--font {font name}`

Sets the font for the display, in CSS format. A typical font selection
might be

    bold 16pt "Liberation Mono"

`h,--host {hostname}`

Set the initial `xineserver` hostname. This can be changed using
`File-\>Connect`.

`p,--port {port number}`

Set the initial `xineserver` IP port. This can be changed using
`File-\>Connect`.

`-r,--readonly`

Enable read-only mode. All menu actions concerned with saving
files are disabled, and the user RC file -- if it exists -- is not
overwritten by command-line values. This option is intended for use
in embedded or kiosk systems, where the main filesystem is mounted
read-only.

`-s,--single`

Enable single-click mode. In this mode, station selection is accomplished
by single-clicking an entry in the station list, rather than
double-clicking, which is the default. Single-click
mode is much more convenient on small touch-screens, which are intended
to be operated by finger-poking. However, in all other circumstances
it is a nuisance, because it becomes too easy to activate a station
accidentally when scrolling. 

Because single-click mode is stored in the user RC file, and there is
no "no single click" command-line option, if this option is given it
will stay in effect until changed in the Preferences dialog. 

## 'Get stations' script

The Perl script `gxsradio_get_stations.pl` -- which is installed in
`/usr/share/gxsradio` by default -- is an example of a script that queries the
`radio-browser.info` database and generates a number of stations files for
`gxsradio`. It is intended to be customized to suit a user's preferences -- by
default it extracts English or English-language stations that have various
tags. 

## Styling

In principle, `gsxradio` should not be styled individually -- style 
themes are a desktop responsibility. However, in an embedded application
there may be no desktop, so it's possible to specify a style file in
CSS format using the `--css` option. The is likely to be particularly
useful for touch-screen operation, because CSS `margin` and `padding`
settings can be used to increase the size of display elements, without
necessarily increasing the font size of text within them. The main
menu bar and the station list header bar (which contains the sorting
buttons) are particular likely to benefit from this treatment.

A sample `.css` file is provided in the `share` directory, that 
demonstrates which elements can be styled. Note that there's
no point setting fonts in a CSS file, because fonts are applied
separately, and to the entire application, using the preferences
dialog box. However, you can specify a font on the command
line using `--font` if required.

## Notes

`gxsradio` does not support condensed or wide fonts, even though the font
selection dialog box shows them. This is because the CSS "font:" style does
not allow the condensed/wide status to be specified. It would be possible
to add an additional entry to the configuration file and the command line,
but it seems a lot of work for little gain. The real culprit here is CSS, and
GTK's ongoing mismatch between the CSS styles that developers are now
supposed to use, and the Pango styles that a lot of the GTK widgets still
use. 

If the font is changed at runtime, the display may not redraw perfectly. The
station list in particular seems to get in a muddle. Restarting the application
should fix this.

'Selected' status of stations is part of the data file, and will be saved
when exiting the program, along with any other changes. It follows that if
you do "Save selected..." then the resulting data file will have _all_ its
rows selected.

The purpose of the 'selected' checkbox is intentionally weakly defined.
It can be used to mark specific filenames to be saved, or just to indicate
stations that have been found to be working (or not). Or for any other 
purpose.

`gxsradio` is designed to work with a filtered list of known-good
radio stations. In principle, on a powerful desktop system it will
load a stations file with tens of thousands of entries; but there's no
useful way to see or manage a list this long. In practice, a list of
several hundreds stations works well, even on a Raspberry Pi.

The station list window supports 'kinetic' scrolling, which can be
very useful on a touchscreen. For the most part, touch-screen 
scrolling doesn't seem to conflict with single-click station selection
-- GTK3 is smart enough to know the difference between a "click"
touch and a "scroll" touch, at least on the touchscreens I've tested.
However, I'm not sure this can be guaranteed with all touchscreens
-- some experimentation might be required.

## Station list file format

Station list files are text files consisting of lines of arbitrary length,
each of which has four fields separated by a tab character. Each 
line represents a station.

The fields are as follows.

    name {tab} country_code {tab} URI {tab} selected

The `country_code` field is intended to contain a two-letter country
code, but `gxsradio` does not really process this field, except as a way
to sort the station list. `selected` is 0 or 1, according to whether
the station has it's check-box checked in the user interface.

The ordering of stations in the file is not usually important, as the
data can be sorted in the application.

## Author and legal

`xsradio` is copyright (c)2020 Kevin Boone, and distributed under the
terms of the GNU Public Licence, version 3.0. Essentially that means
you may use the software however you wish, so long as the source
code continues to be made available, and the original author is
acknowledged. There is no warranty of any kind.

Sample station list files contain data that is extracted from the Community
Radio Station Index project, maintained by Alex Segler and others. 


## Revision history

0.1a Feb 2020<br/>
First working version 

0.1b Feb 2020<br/>
Added 'single-click' mode to the station list
Various bug fixes



