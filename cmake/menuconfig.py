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
    """Disable boolean symbols made unreachable by turning off one symbol."""
    changed = True
    values[name] = False
    while changed:
        changed = False
        for dependent, symbol in symbols.items():
            dependency_disabled = (
                symbol.dependency and not values[symbol.dependency]
            )
            if (symbol.kind == "bool" and values[dependent]
                    and dependency_disabled):
                values[dependent] = False
                changed = True


def render_value(symbol, value):
    """Format one effective value for the menu and detail panel."""
    if symbol.kind == "bool":
        return "[*]" if value else "[ ]"
    if symbol.kind == "string":
        return '"{}"'.format(value)
    if symbol.kind == "hex":
        return "0x{:x}".format(value)
    return str(value)


def symbol_group(source_dir, symbol):
    """Use the defining directory as the symbol's visible menu group."""
    try:
        relative = symbol.path.relative_to(source_dir)
    except ValueError:
        relative = symbol.path
    parent = relative.parent
    if str(parent) == ".":
        return "General"
    return str(parent)


def symbol_location(source_dir, symbol):
    """Prefer a source-relative location for the detail panel."""
    try:
        return symbol.path.relative_to(source_dir)
    except ValueError:
        return symbol.path


def build_entries(source_dir, symbols):
    """Interleave directory headings with source-ordered symbol entries."""
    entries = []
    last_group = None
    ordered_symbols = sorted(
        symbols.values(), key=lambda symbol: (str(symbol.path), symbol.line)
    )
    for symbol in ordered_symbols:
        name = symbol.name
        group = symbol_group(source_dir, symbol)
        if group != last_group:
            entries.append(("heading", group))
            last_group = group
        entries.append(("symbol", name))
    return entries


def selectable_indices(entries):
    """Return menu indices that represent editable symbols."""
    return [index for index, entry in enumerate(entries) if entry[0] == "symbol"]


def next_selectable(entries, selected, step):
    """Move one direction without landing on a group heading."""
    index = selected + step
    while 0 <= index < len(entries):
        if entries[index][0] == "symbol":
            return index
        index += step
    return selected


def clamp_selectable(entries, selected):
    """Clamp page movement to the nearest editable entry."""
    if entries[selected][0] == "symbol":
        return selected
    for index in range(selected + 1, len(entries)):
        if entries[index][0] == "symbol":
            return index
    for index in range(selected - 1, -1, -1):
        if entries[index][0] == "symbol":
            return index
    return selected


def search_entries(entries, symbols, query):
    """Return symbol entry indices matching a case-insensitive query."""
    matches = []
    needle = query.casefold()
    for index, entry in enumerate(entries):
        if entry[0] == "symbol":
            symbol = symbols[entry[1]]
            fields = (
                "CONFIG_{}".format(symbol.name),
                symbol.prompt,
                symbol.kind,
                str(symbol.path),
            )
            if any(needle in field.casefold() for field in fields):
                matches.append(index)
    return matches


def search_match(matches, selected, step, include_selected=False):
    """Find a matching entry in one direction and wrap at either end."""
    result = selected
    if matches:
        if step > 0:
            candidates = [index for index in matches
                          if index > selected or
                          (include_selected and index == selected)]
            result = candidates[0] if candidates else matches[0]
        else:
            candidates = [index for index in matches
                          if index < selected or
                          (include_selected and index == selected)]
            result = candidates[-1] if candidates else matches[-1]
    return result


def color_pair(pair):
    """Return a color pair only when the terminal supports colors."""
    if curses.has_colors():
        return curses.color_pair(pair)
    return curses.A_NORMAL


def draw_text(screen, row, column, text, width, attributes=curses.A_NORMAL):
    """Clip terminal output and tolerate writes at a resized screen edge."""
    if width <= 0:
        return
    try:
        screen.addnstr(row, column, text, width, attributes)
    except curses.error:
        pass


def draw_bar(screen, row, text, attributes):
    """Draw one full-width status or key-help bar."""
    height, width = screen.getmaxyx()
    if 0 <= row < height:
        draw_text(screen, row, 0, text.ljust(width), max(1, width - 1), attributes)


def read_input(screen, prompt):
    """Read one status-line value while preserving normal cursor settings."""
    height, width = screen.getmaxyx()
    column = min(len(prompt), width - 1)
    maximum = max(1, width - column - 1)
    screen.move(height - 2, 0)
    screen.clrtoeol()
    screen.addnstr(height - 2, 0, prompt, max(1, width - 1))
    curses.echo()
    try:
        try:
            curses.curs_set(1)
        except curses.error:
            pass
        raw = screen.getstr(height - 2, column, maximum).decode("utf-8")
    finally:
        curses.noecho()
        try:
            curses.curs_set(0)
        except curses.error:
            pass
    return raw


def edit_value(screen, symbol, current, row):
    """Read and validate a non-boolean value on the status line."""
    prompt = "CONFIG_{}: ".format(symbol.name)
    raw = read_input(screen, prompt)
    if symbol.kind == "string":
        raw = kconfig.quoted(raw)
    return kconfig.parse_value(symbol.kind, raw, symbol.path, row)


def draw_help(screen, source_dir, symbol, values, top, left, height, width):
    """Draw the selected symbol's definition and dependency details."""
    if width < 12:
        return
    panel_attr = color_pair(4)
    title_attr = color_pair(5) | curses.A_BOLD
    for row in range(top, top + height):
        draw_text(screen, row, left, " " * width, width, panel_attr)
    draw_text(screen, top, left + 1, "Symbol", width - 2, title_attr)
    lines = [
        "CONFIG_{}".format(symbol.name),
        "",
        "Prompt: {}".format(symbol.prompt),
        "Type: {}".format(symbol.kind),
        "Value: {}".format(render_value(symbol, values[symbol.name])),
        "Defined at: {}:{}".format(symbol_location(source_dir, symbol), symbol.line),
    ]
    if symbol.default is not None:
        lines.append("Default: {}".format(symbol.default))
    if symbol.dependency:
        lines.append("Depends on: CONFIG_{}".format(symbol.dependency))
        lines.append("Dependency value: {}".format(
            "y" if values[symbol.dependency] else "n"
        ))
    row = top + 2
    for line in lines:
        if row >= top + height:
            break
        draw_text(screen, row, left + 1, line, width - 2, panel_attr)
        row += 1


def menu(screen, source_dir, symbols, values):
    """Run the nconfig-style editor and return whether changes should save."""
    entries = build_entries(source_dir, symbols)
    selectable = selectable_indices(entries)
    if not selectable:
        return False
    selected = selectable[0]
    offset = 0
    status = "Ready"
    search_query = ""
    search_matches = []
    try:
        curses.start_color()
        curses.use_default_colors()
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_CYAN)
        curses.init_pair(2, curses.COLOR_CYAN, -1)
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)
        curses.init_pair(4, curses.COLOR_WHITE, curses.COLOR_BLUE)
        curses.init_pair(5, curses.COLOR_YELLOW, curses.COLOR_BLUE)
        curses.init_pair(6, curses.COLOR_WHITE, curses.COLOR_BLACK)
    except curses.error:
        pass
    try:
        curses.curs_set(0)
    except curses.error:
        pass
    screen.keypad(True)
    while True:
        # Derive the viewport on every iteration so terminal resize is safe.
        height, width = screen.getmaxyx()
        if height < 8 or width < 48:
            screen.erase()
            draw_text(screen, 0, 0, "Terminal too small for menuconfig", width - 1)
            screen.refresh()
            key = screen.getch()
            if key in (ord("q"), ord("Q")):
                return False
            continue
        visible = max(1, height - 5)
        if selected < offset:
            offset = selected
        elif selected >= offset + visible:
            offset = selected - visible + 1
        if offset > 0 and entries[offset][0] == "symbol":
            while offset > 0 and entries[offset - 1][0] == "symbol":
                offset -= 1
                if selected - offset >= visible:
                    offset += 1
                    break
        detail_width = max(24, min(44, width // 3))
        list_width = width - detail_width - 1

        # Paint a complete frame to avoid stale text after resize or scrolling.
        screen.erase()
        draw_bar(screen, 0, " simple-boot nconfig", color_pair(1) | curses.A_BOLD)
        draw_bar(screen, 1, " " + status, color_pair(6))
        draw_text(screen, 2, 0, "Configuration", list_width, curses.A_BOLD)
        draw_text(screen, 2, list_width, " ", 1)
        for index in range(offset, min(len(entries), offset + visible)):
            entry = entries[index]
            row = index - offset + 3
            if entry[0] == "heading":
                line = "  {}  ".format(entry[1])
                draw_text(screen, row, 0, line, list_width, color_pair(2) | curses.A_BOLD)
                continue
            name = entry[1]
            symbol = symbols[name]
            enabled = not symbol.dependency or values[symbol.dependency]
            marker = "-->" if index == selected else "   "
            line = "{} {:<10} {:<34} {}".format(
                marker, render_value(symbol, values[name]),
                "CONFIG_{}".format(name), symbol.prompt
            )
            attributes = curses.A_REVERSE if index == selected else curses.A_NORMAL
            if not enabled:
                line += " (requires CONFIG_{})".format(symbol.dependency)
                attributes |= curses.A_DIM
            if index == selected:
                attributes |= color_pair(3)
            draw_text(screen, row, 0, line, list_width, attributes)
        selected_name = entries[selected][1]
        draw_help(screen, source_dir, symbols[selected_name], values,
                  2, list_width + 1, height - 4, detail_width - 1)
        draw_bar(screen, height - 2,
                 "  <Up/Down> Move  <Space/Enter> Edit  <S> Save  <Q> Quit",
                 color_pair(1) | curses.A_BOLD)
        draw_bar(screen, height - 1,
                 "  </> Search  <n/N> Match  <PgUp/PgDn> Page",
                 color_pair(1))
        screen.refresh()

        # Key dispatch mutates only selection, values, status, or save intent.
        key = screen.getch()
        if key in (curses.KEY_UP, ord("k")):
            selected = next_selectable(entries, selected, -1)
        elif key in (curses.KEY_DOWN, ord("j")):
            selected = next_selectable(entries, selected, 1)
        elif key in (curses.KEY_NPAGE,):
            selected = clamp_selectable(entries, min(len(entries) - 1,
                                                     selected + visible))
        elif key in (curses.KEY_PPAGE,):
            selected = clamp_selectable(entries, max(0, selected - visible))
        elif key in (curses.KEY_HOME, ord("g")):
            selected = selectable[0]
        elif key in (curses.KEY_END, ord("G")):
            selected = selectable[-1]
        elif key == ord("/"):
            query = read_input(screen, "Search: ").strip()
            if query:
                matches = search_entries(entries, symbols, query)
                if matches:
                    search_query = query
                    search_matches = matches
                    selected = search_match(matches, selected, 1, True)
                    position = matches.index(selected) + 1
                    status = "Search /{}: {}/{}".format(
                        query, position, len(matches)
                    )
                else:
                    search_query = query
                    search_matches = []
                    status = "No symbols match /{}".format(query)
            else:
                status = "Search cancelled"
        elif key in (ord("n"), ord("N")):
            if search_matches:
                step = 1 if key == ord("n") else -1
                selected = search_match(search_matches, selected, step)
                position = search_matches.index(selected) + 1
                status = "Search /{}: {}/{}".format(
                    search_query, position, len(search_matches)
                )
            else:
                status = "No active search; press /"
        elif key in (ord("q"), ord("Q")):
            return False
        elif key in (ord("s"), ord("S")):
            return True
        elif key in (ord(" "), curses.KEY_ENTER, 10, 13):
            name = entries[selected][1]
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
    """Load merged configuration, run the editor, and save local changes."""
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
        if not curses.wrapper(menu, source_dir, symbols, values):
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
