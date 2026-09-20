# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/menuconfig.py
# Brief: Edit the merged build configuration using a terminal menu.

# Imports --------------------------------------------------------------------
from __future__ import print_function

import argparse
import curses
from pathlib import Path

import kconfig


# Menu operations ------------------------------------------------------------
def disable_dependents(symbols, values, name):
    changed = True
    while changed:
        changed = False
        for dependent, symbol in symbols.items():
            if symbol.dependency == name and values[dependent]:
                if symbol.kind == "bool":
                    values[dependent] = False
                    changed = True
            if symbol.dependency and not values[symbol.dependency] and values[dependent]:
                if symbol.kind == "bool":
                    values[dependent] = False
                    changed = True


def render_value(symbol, value):
    if symbol.kind == "bool":
        return "[*]" if value else "[ ]"
    if symbol.kind == "string":
        return '"{}"'.format(value)
    if symbol.kind == "hex":
        return "0x{:x}".format(value)
    return str(value)


def edit_value(screen, symbol, current, row):
    height, width = screen.getmaxyx()
    prompt = "CONFIG_{}: ".format(symbol.name)
    screen.move(height - 2, 0)
    screen.clrtoeol()
    screen.addnstr(height - 2, 0, prompt, max(1, width - 1))
    curses.echo()
    try:
        raw = screen.getstr(height - 2, min(len(prompt), width - 1)).decode("utf-8")
    finally:
        curses.noecho()
    if symbol.kind == "string":
        raw = kconfig.quoted(raw)
    return kconfig.parse_value(symbol.kind, raw, symbol.path, row)


def menu(screen, symbols, values):
    names = sorted(symbols)
    selected = 0
    offset = 0
    status = "Arrows: move  Space/Enter: edit  S: save  Q: quit"
    try:
        curses.curs_set(0)
    except curses.error:
        pass
    screen.keypad(True)
    while True:
        height, width = screen.getmaxyx()
        visible = max(1, height - 4)
        if selected < offset:
            offset = selected
        elif selected >= offset + visible:
            offset = selected - visible + 1
        screen.erase()
        screen.addnstr(0, 0, "simple-boot menuconfig", max(1, width - 1), curses.A_BOLD)
        screen.addnstr(1, 0, status, max(1, width - 1))
        for index in range(offset, min(len(names), offset + visible)):
            name = names[index]
            symbol = symbols[name]
            enabled = not symbol.dependency or values[symbol.dependency]
            line = "{:<12} CONFIG_{} - {}".format(
                render_value(symbol, values[name]), name, symbol.prompt
            )
            attributes = curses.A_REVERSE if index == selected else curses.A_NORMAL
            if not enabled:
                line += " (requires CONFIG_{})".format(symbol.dependency)
                attributes |= curses.A_DIM
            screen.addnstr(index - offset + 2, 0, line, max(1, width - 1), attributes)
        screen.refresh()
        key = screen.getch()
        if key in (curses.KEY_UP, ord("k")):
            selected = max(0, selected - 1)
        elif key in (curses.KEY_DOWN, ord("j")):
            selected = min(len(names) - 1, selected + 1)
        elif key in (curses.KEY_NPAGE,):
            selected = min(len(names) - 1, selected + visible)
        elif key in (curses.KEY_PPAGE,):
            selected = max(0, selected - visible)
        elif key in (ord("q"), ord("Q")):
            return False
        elif key in (ord("s"), ord("S")):
            return True
        elif key in (ord(" "), curses.KEY_ENTER, 10, 13):
            name = names[selected]
            symbol = symbols[name]
            if symbol.dependency and not values[symbol.dependency]:
                status = "CONFIG_{} requires CONFIG_{}".format(name, symbol.dependency)
                continue
            if symbol.kind == "bool":
                values[name] = not values[name]
                if not values[name]:
                    disable_dependents(symbols, values, name)
                status = "Updated CONFIG_{}".format(name)
            else:
                try:
                    values[name] = edit_value(screen, symbol, values[name], selected + 1)
                    status = "Updated CONFIG_{}".format(name)
                except ValueError as error:
                    status = str(error)


# Command-line entry ---------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(description="Edit simple-boot configuration.")
    parser.add_argument("--source-dir", type=Path, required=True)
    parser.add_argument("--kconfig", type=Path, required=True)
    parser.add_argument("--base-config", type=Path, action="append", required=True)
    parser.add_argument("--current-config", type=Path, required=True)
    parser.add_argument("--local-config", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    try:
        source_dir = args.source_dir.resolve()
        symbols, files = kconfig.parse_kconfig(source_dir, args.kconfig.resolve())
        base_configs = [path.resolve() for path in args.base_config]
        for path in base_configs:
            if not path.is_file():
                raise ValueError("configuration file does not exist: {}".format(path))
        base_values = kconfig.merge_configs(base_configs, symbols)
        if args.current_config.is_file():
            values = kconfig.merge_configs([args.current_config.resolve()], symbols)
        else:
            values = dict(base_values)
        if not curses.wrapper(menu, symbols, values):
            return
        kconfig.validate_dependencies(symbols, values)
        names = kconfig.changed_names(symbols, values, base_values)
        kconfig.atomic_write(args.local_config, kconfig.config_text(
            symbols, values, names,
            "# Local menuconfig changes relative to the selected board configs."
        ))
        kconfig.generate_outputs(args.output_dir, symbols, values, files)
        print("Saved {} and {}".format(args.current_config, args.local_config))
    except (OSError, ValueError, curses.error) as error:
        parser.exit(1, "menuconfig: {}\n".format(error))


if __name__ == "__main__":
    main()
