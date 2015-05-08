#!/bin/sh

printf '\033[0;32m%s\033[0m\n'   " _   _      _               _       _  "
printf '\033[0;32m%s\033[0m\n'   "| | | |    (_)             | |     | |"
printf '\033[0;32m%s\033[0m\n'   "| |_| | ___ _ _ __ ___   __| | __ _| |"
printf '\033[0;32m%s\033[0m\n'   "|  _  |/ _ \ | '_ \` _ \ / _\` |/ _\` | |"
printf '\033[0;32m%s\033[0m\n'   "| | | |  __/ | | | | | | (_| | (_| | |"
printf '\033[0;32m%s\033[0m\n'   "\_| |_/\___|_|_| |_| |_|\__,_|\__,_|_|"

printf '\033[0;31m%s\n\n\n'   "Prepare to face your doom"
cd ~/robot-desafio/Raspberry-pi
python2.7 engine.py
