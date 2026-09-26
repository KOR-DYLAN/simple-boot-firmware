# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/kconfig.py
# Brief: Parse the project KConfig subset and emit CMake and C configuration.

# Imports --------------------------------------------------------------------
from __future__ import print_function

import argparse
import ast
import os
from pathlib import Path
import re
import tempfile


# Parsing helpers ------------------------------------------------------------
SYMBOL_PATTERN = re.compile(r"^[A-Z][A-Z0-9_]*$")
CONFIG_PATTERN = re.compile(r"^CONFIG_([A-Z][A-Z0-9_]*)=(.*)$")
NOT_SET_PATTERN = re.compile(r"^# CONFIG_([A-Z][A-Z0-9_]*) is not set$")
SOURCE_PATTERN = re.compile(r'^source\s+"([^"]+)"$')
TYPE_PATTERN = re.compile(r'^(bool|string|int|hex|value)(?:\s+"([^"]*)")?$')
DEPEND_PATTERN = re.compile(r"^depends on ([A-Z][A-Z0-9_]*)$")


class Symbol(object):
    """One parsed KConfig symbol and the source location that defines it."""

    def __init__(self, name, path, line):
        self.name = name
        self.path = path
        self.line = line
        self.kind = None
        self.prompt = name.replace("_", " ").title()
        self.default = None
        self.dependency = None


def fail(path, line, message):
    """Raise a source-qualified configuration error."""
    raise ValueError("{}:{}: {}".format(path, line, message))


def decode_string(raw, path, line):
    """Decode one quoted KConfig string without accepting other literals."""
    try:
        value = ast.literal_eval(raw)
    except (SyntaxError, ValueError):
        fail(path, line, "invalid quoted string")
    if not isinstance(value, str):
        fail(path, line, "string value must be quoted")
    return value


def parse_value(kind, raw, path, line):
    """Convert source text to the normalized value for a symbol type."""
    raw = raw.strip()
    if kind == "bool":
        values = {"y": True, "n": False, "1": True, "0": False,
                  "yes": True, "no": False, "true": True, "false": False}
        key = raw.lower()
        if key not in values:
            fail(path, line, "boolean value must be y or n")
        return values[key]
    if kind == "string":
        return decode_string(raw, path, line)
    if kind in ("int", "hex"):
        try:
            value = int(raw, 0)
        except ValueError:
            fail(path, line, "{} value must be numeric".format(kind))
        if kind == "hex" and value < 0:
            fail(path, line, "hex value must not be negative")
        return value
    if not raw or any(character.isspace() for character in raw):
        fail(path, line, "value must be a single token")
    return raw


def parse_kconfig(root, entry):
    """Parse the supported KConfig subset and all recursive source entries."""
    symbols = {}
    files = []
    active = set()

    def visit(path):
        """Visit one KConfig file while rejecting recursive source cycles."""
        path = path.resolve()
        if path in active:
            raise ValueError("recursive KConfig source: {}".format(path))
        if not path.is_file():
            raise ValueError("KConfig source does not exist: {}".format(path))
        active.add(path)
        files.append(path)
        current = None
        for number, original in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            line = original.strip()
            if not line or line.startswith("#"):
                continue
            source = SOURCE_PATTERN.match(line)
            if source:
                visit(root / source.group(1))
                current = None
                continue
            if line.startswith("menu ") or line == "endmenu":
                current = None
                continue
            if line.startswith("config "):
                name = line[7:].strip()
                if not SYMBOL_PATTERN.match(name):
                    fail(path, number, "invalid config symbol")
                if name in symbols:
                    fail(path, number, "duplicate config symbol {}".format(name))
                current = Symbol(name, path, number)
                symbols[name] = current
                continue
            if current is None:
                fail(path, number, "statement outside a config block")
            kind = TYPE_PATTERN.match(line)
            if kind:
                if current.kind is not None:
                    fail(path, number, "duplicate symbol type")
                current.kind = kind.group(1)
                if kind.group(2):
                    current.prompt = kind.group(2)
                continue
            if line.startswith("default "):
                current.default = line[8:].strip()
                continue
            dependency = DEPEND_PATTERN.match(line)
            if dependency:
                current.dependency = dependency.group(1)
                continue
            fail(path, number, "unsupported KConfig statement")
        active.remove(path)

    visit(entry)
    for symbol in symbols.values():
        if symbol.kind is None:
            fail(symbol.path, symbol.line, "config symbol has no type")
        if symbol.dependency and symbol.dependency not in symbols:
            fail(symbol.path, symbol.line, "unknown dependency {}".format(symbol.dependency))
    return symbols, files


def parse_config(path, symbols):
    """Parse explicit CONFIG assignments from one configuration file."""
    assignments = {}
    for number, original in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = original.strip()
        if not line:
            continue
        disabled = NOT_SET_PATTERN.match(line)
        assignment = CONFIG_PATTERN.match(line)
        if disabled:
            name, raw = disabled.group(1), "n"
        elif assignment:
            name, raw = assignment.groups()
        elif line.startswith("#"):
            continue
        else:
            fail(path, number, "expected CONFIG_<name>=<value>")
        if name not in symbols:
            fail(path, number, "unknown symbol CONFIG_{}".format(name))
        if name in assignments:
            fail(path, number, "duplicate assignment CONFIG_{}".format(name))
        assignments[name] = parse_value(symbols[name].kind, raw, path, number)

    return assignments


def default_values(symbols):
    """Resolve every symbol to its declared or type-derived default."""
    values = {}
    for name, symbol in symbols.items():
        if symbol.default is not None:
            values[name] = parse_value(symbol.kind, symbol.default,
                                       symbol.path, symbol.line)
        elif symbol.kind == "bool":
            values[name] = False
        else:
            fail(symbol.path, symbol.line, "CONFIG_{} has no value".format(name))
    return values


def validate_dependencies(symbols, values):
    """Ensure enabled values have all of their direct dependencies enabled."""
    for name, symbol in symbols.items():
        if symbol.dependency and values[name] and not values[symbol.dependency]:
            raise ValueError("CONFIG_{} requires CONFIG_{}".format(
                name, symbol.dependency))


def merge_configs(paths, symbols):
    """Overlay configuration files in order on top of symbol defaults."""
    values = default_values(symbols)
    for path in paths:
        values.update(parse_config(path, symbols))
    validate_dependencies(symbols, values)
    return values


# Output generation ----------------------------------------------------------
def atomic_write(path, content):
    """Replace generated text atomically and leave unchanged files untouched."""
    if path.is_file() and path.read_text(encoding="utf-8") == content:
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary_name = tempfile.mkstemp(dir=str(path.parent))
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8") as output:
            output.write(content)
        os.replace(temporary_name, str(path))
    finally:
        if os.path.exists(temporary_name):
            os.unlink(temporary_name)


def quoted(value):
    """Quote a string for project configuration syntax."""
    return '"{}"'.format(value.replace("\\", "\\\\").replace('"', '\\"'))


def config_text(symbols, values, names=None, heading=None):
    """Render a complete config or a selected-symbol fragment."""
    lines = [heading or "# Generated by cmake/kconfig.py. Do not edit."]
    selected_names = set(names) if names is not None else symbols
    for name in sorted(selected_names):
        kind = symbols[name].kind
        value = values[name]
        if kind == "bool":
            lines.append("CONFIG_{}={}".format(name, "y" if value else "n"))
        elif kind == "string":
            lines.append("CONFIG_{}={}".format(name, quoted(value)))
        elif kind == "hex":
            lines.append("CONFIG_{}=0x{:x}".format(name, value))
        else:
            lines.append("CONFIG_{}={}".format(name, value))
    return "\n".join(lines) + "\n"


def changed_names(symbols, values, base_values):
    """Return symbols whose effective values differ from a baseline."""
    return [name for name in symbols if values[name] != base_values[name]]


def save_delta(path, symbols, values, base_values, heading):
    """Save each value that differs from a baseline exactly once."""
    names = changed_names(symbols, values, base_values)
    atomic_write(path, config_text(symbols, values, names, heading))


def generate_outputs(output_dir, symbols, values, files):
    """Publish the C, CMake, and canonical text configuration outputs."""
    atomic_write(output_dir / "autoconf.h", header_text(symbols, values))
    atomic_write(output_dir / "config.cmake", cmake_text(symbols, values, files))
    atomic_write(output_dir.parent / ".config", config_text(symbols, values))


def header_text(symbols, values):
    """Render generated preprocessor definitions for target compilation."""
    lines = ["/* Generated by cmake/kconfig.py. Do not edit. */",
             "#ifndef SIMPLE_BOOT_AUTOCONF_H",
             "#define SIMPLE_BOOT_AUTOCONF_H"]
    for name in sorted(symbols):
        kind = symbols[name].kind
        value = values[name]
        if kind == "bool":
            if value:
                lines.append("#define CONFIG_{} 1".format(name))
            else:
                lines.append("/* #undef CONFIG_{} */".format(name))
        elif kind == "string":
            lines.append("#define CONFIG_{} {}".format(name, quoted(value)))
        elif kind == "hex":
            lines.append("#define CONFIG_{} 0x{:x}".format(name, value))
        else:
            lines.append("#define CONFIG_{} {}".format(name, value))
    lines.extend(["#endif /* SIMPLE_BOOT_AUTOCONF_H */", ""])
    return "\n".join(lines)


def cmake_escape(value):
    """Escape one scalar for a quoted CMake argument."""
    return str(value).replace("\\", "\\\\").replace('"', '\\"').replace(";", "\\;")


def cmake_text(symbols, values, files):
    """Render generated CMake values and configure dependencies."""
    names = ["CONFIG_{}".format(name) for name in sorted(symbols)]
    lines = ["# Generated by cmake/kconfig.py. Do not edit.",
             "set(BOOT_KCONFIG_SYMBOLS {})".format(" ".join(names)),
             "set(BOOT_KCONFIG_FILES"]
    lines.extend('    "{}"'.format(cmake_escape(path)) for path in files)
    lines.append(")")
    for name in sorted(symbols):
        value = values[name]
        if symbols[name].kind == "bool":
            rendered = "ON" if value else "OFF"
        else:
            rendered = '"{}"'.format(cmake_escape(value))
        lines.append("set(CONFIG_{} {})".format(name, rendered))
    return "\n".join(lines) + "\n"


# Command-line entry ---------------------------------------------------------
def main():
    """Parse configuration inputs and write the requested output forms."""
    parser = argparse.ArgumentParser(
        description="Parse project KConfig files and generate build configuration."
    )
    parser.add_argument("--source-dir", type=Path, required=True)
    parser.add_argument("--kconfig", type=Path, required=True)
    parser.add_argument("--config", type=Path, action="append", required=True)
    parser.add_argument("--base-config", type=Path, action="append", default=[])
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--clear-local", type=Path)
    parser.add_argument("--save-defconfig", type=Path)
    parser.add_argument("--save-fragment", type=Path)
    args = parser.parse_args()
    try:
        source_dir = args.source_dir.resolve()
        if args.clear_local and args.clear_local.exists():
            args.clear_local.unlink()
        configs = [path.resolve() for path in args.config]
        base_configs = [path.resolve() for path in args.base_config]
        for config in configs + base_configs:
            if not config.is_file():
                raise ValueError("configuration file does not exist: {}".format(config))
        symbols, files = parse_kconfig(source_dir, args.kconfig.resolve())
        values = merge_configs(configs, symbols)
        generate_outputs(args.output_dir, symbols, values, files)
        if args.save_defconfig:
            defaults = default_values(symbols)
            save_delta(
                args.save_defconfig, symbols, values, defaults,
                "# Minimal configuration relative to KConfig defaults."
            )
        if args.save_fragment:
            if not base_configs:
                raise ValueError("--save-fragment requires --base-config")
            base_values = merge_configs(base_configs, symbols)
            save_delta(
                args.save_fragment, symbols, values, base_values,
                "# Configuration fragment relative to selected base configs."
            )
    except (OSError, ValueError) as error:
        parser.exit(1, "kconfig: {}\n".format(error))


if __name__ == "__main__":
    main()
