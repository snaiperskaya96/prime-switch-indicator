# Prime Switch Indicator

A badly coded Prime indicator for Gnome 3

![Preview](https://raw.githubusercontent.com/snaiperskaya96/prime-switch-indicator/master/media/preview.png)

## How it works

Once built and installed, you'll have a new (user) prime-switch-indicator.service for systemd (I know, I know...).


This services makes sure you always have the appindicator up and running so you don't have to manually start it every time.

The appindicator abuses of a suid-abuser wrapper around prime-select, that is, we literally build a program (prime-switch-indicator-select) whose only reason to exist is to prevent password requests for every time we want to switch gpu.

So in the end, those files will be copied around your system:
  - /usr/bin/prime-switch-indicator
  - /usr/bin/prime-switch-indicator-select
  - /etc/systemd/user/prime-switch-indicator.service
  - /usr/share/icons/prime-switch-indicator/indicator.png 
  
## Build

You'll need to build this guy by yourself.
Make sure you have cmake, libgtk-3-dev and libappindicator3-dev (You probably want git as well my dude?).

In APT language this translates to:
```bash
sudo apt install libgtk-3-dev libappindicator3-dev cmake git
```

Once You got everything installed, make a build directory inside the root folder, run `cmake ..` and `sudo make install` and You're golden.

```bash
cd /tmp/
git clone https://github.com/snaiperskaya96/prime-switch-indicator.git
cd prime-switch-indicator
mkdir build && cd build
cmake ..
sudo make install
```

## Install

The last thing You'll likely do is to setup systemd (ew) so it can start the app indicator automagically when You login in your beautiful Gnome desktop. To do such, just run:
```bash
systemctl --user enable prime-switch-indicator.service
systemctl --user start prime-switch-indicator.service
```

An NVIDIA-like icon should appear on your indicator bar!

Enjoy and feel free to make this thing better by contributing :kissing_closed_eyes: