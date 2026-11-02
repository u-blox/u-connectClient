#!/usr/bin/env python3
"""Merge the generated APIs of several u-connectXpress modules into one superset API.

Each folder in ucx_api/generated/ covers one module. An application that drives more than
one module (NORA-W36 and NORA-B26, say) builds against the merged folder (UCX_MODULE=MERGED)
and finds out at run time which commands the connected module answers.

The merge works on top-level C items (functions, prototypes, typedefs, preprocessor lines):
- an item only one module has is added, next to its neighbour in that module's file;
- a function both have must generate the same code (comments may differ);
- an enum both have must have the same values, one may only add members;
- a struct both have (uUrcCallbacks) gets the union of its members;
- the URC dispatch function (uCxUrcParse) gets the union of its branches.
Anything else is a conflict and stops the merge.aeffs

Run it after updating a generated folder:
    python ucx_api/merge_api.py           # writes ucx_api/generated/MERGED
    python ucx_api/merge_api.py --check   # fails when MERGED is not up to date
"""

import argparse
import re
import sys
from pathlib import Path

GENERATED = Path(__file__).resolve().parent / "generated"
DEFAULT_MODULES = ("NORA-W36X", "NORA-B26X")
OUTPUT = "MERGED"


class MergeError(Exception):
    pass


def _code(text):
    """Text without comments and string literals, for brace counting and comparisons."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    return re.sub(r'"(?:\\.|[^"\\])*"', '""', text)


def _norm(text):
    return re.sub(r"\s+", " ", _code(text)).strip()


def split_items(text):
    """Top-level items as (key, text); comments and blank lines belong to the item after them."""
    items, buf, depth, in_comment, prev = [], [], 0, False, ""
    seen = {}

    def emit(key):
        n = seen.get(key, 0)
        seen[key] = n + 1
        items.append(((key, n), "".join(buf)))
        buf.clear()

    for line in text.splitlines(keepends=True):
        buf.append(line)
        stripped = line.strip()
        if in_comment:
            in_comment = "*/" not in stripped
            continue
        if depth == 0:
            if stripped == "" or stripped.startswith("//"):
                continue
            if stripped.startswith("/*"):
                in_comment = "*/" not in stripped
                continue
            # #directives, and the extern "C" { ... } lines between #ifdef __cplusplus and #endif
            if stripped.startswith("#") or (prev == "#ifdef __cplusplus" and stripped in ('extern "C" {', "}")):
                emit(("pp", stripped))
                prev = stripped
                continue
        code = _code(line)
        depth += code.count("{") - code.count("}")
        if depth < 0:
            raise MergeError(f"unbalanced braces at: {line.strip()}")
        if depth == 0 and code.strip().endswith((";", "}")):
            emit(_item_key("".join(buf)))
        prev = stripped
    if buf:
        items.append(((("tail", ""), 0), "".join(buf)))
    return items


def _item_key(text):
    code = _code(text).strip()
    if code.startswith("typedef"):
        m = re.search(r"\(\s*\*\s*(\w+)\s*\)", code)  # function pointer
        if m and "{" not in code:
            return ("type", m.group(1))
        return ("type", re.search(r"(\w+)\s*;\s*$", code).group(1))
    m = re.search(r"(\w+)\s*\(", code)
    if m:
        return ("func", m.group(1))
    return ("other", _norm(text))


def _enum_members(text):
    body = _code(text)
    body = body[body.index("{") + 1:body.rindex("}")]
    members, value = {}, -1
    for part in (p.strip() for p in body.split(",")):
        if not part:
            continue
        name, _, expr = part.partition("=")
        value = int(expr.strip(), 0) if expr.strip() else value + 1
        members[name.strip()] = value
    return members


def _merge_enum(name, a, b):
    ma, mb = _enum_members(a), _enum_members(b)
    if all(mb.get(k) == v for k, v in ma.items()):
        return b
    if all(ma.get(k) == v for k, v in mb.items()):
        return a
    raise MergeError(f"enum {name} differs: {ma} / {mb}")


def _merge_struct(name, a, b):
    """Members of a, then the members only b has, before the closing brace."""
    def members(text):
        body = text[text.index("{") + 1:text.rindex("}")]
        return [line for line in body.splitlines(keepends=True) if line.strip().endswith(";")]

    have = {_norm(m) for m in members(a)}
    extra = [m for m in members(b) if _norm(m) not in have]
    end = a.rindex("}")
    return a[:end] + "".join(extra) + a[end:]


_BRANCH = re.compile(r"( *if \(strcmp\(pUrcName, \"([^\"]+)\"\) == 0\) \{\n.*?\n *\}\n)", re.S)


def _merge_dispatch(name, a, b):
    """uCxUrcParse: the branches of a, then the URCs only b has, before the final return."""
    have = {m.group(2) for m in _BRANCH.finditer(a)}
    extra = [m.group(1) for m in _BRANCH.finditer(b) if m.group(2) not in have]
    end = a.rindex("    return")
    return a[:end] + "".join(extra) + a[end:]


def _resolve(key, a, b, filename):
    kind, name = key[0]
    if _norm(a) == _norm(b):
        return a
    code = _code(a)
    if kind == "type" and re.match(r"\s*typedef\s+enum", code):
        return _merge_enum(name, a, b)
    if kind == "type" and re.match(r"\s*typedef\s+struct", code):
        return _merge_struct(name, a, b)
    if kind == "func" and _BRANCH.search(a):
        return _merge_dispatch(name, a, b)
    raise MergeError(f"{filename}: {kind} {name} differs between the modules")


def merge_text(texts, filename):
    """Merge one file's text from each module (in priority order)."""
    out = split_items(texts[0])
    for other in texts[1:]:
        prev = None
        for key, text in split_items(other):
            keys = [k for k, _ in out]
            if key in keys:
                i = keys.index(key)
                out[i] = (key, _resolve(key, out[i][1], text, filename))
            else:
                # after the item it follows in its own file, else before the tail
                at = keys.index(prev) + 1 if prev in keys else len(keys) - (keys[-1][0][0] == "tail")
                out.insert(at, (key, text))
            prev = key
    return "".join(text for _, text in out)


def merge(modules):
    """{file name: merged text} for the given generated folders."""
    folders = [GENERATED / m for m in modules]
    for f in folders:
        if not f.is_dir():
            raise MergeError(f"no generated API {f}")
    versions = {(f / "u_cx_version.h").read_text() for f in folders}
    if len(versions) != 1:
        raise MergeError("the modules have different API versions; update them all first")
    names = sorted({p.name for f in folders for p in f.iterdir() if p.suffix in (".c", ".h")})
    note = f"/* Merged from {', '.join(modules)} by ucx_api/merge_api.py; do not edit. */\n"
    result = {}
    for name in names:
        texts = [(f / name).read_text() for f in folders if (f / name).exists()]
        result[name] = note + merge_text(texts, name)
    return result


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", default=DEFAULT_MODULES, help="generated folders, first one wins on comments")
    ap.add_argument("--check", action="store_true", help=f"fail when generated/{OUTPUT} is not up to date")
    args = ap.parse_args(argv)
    try:
        files = merge(args.modules)
    except MergeError as e:
        print(f"merge_api: {e}", file=sys.stderr)
        return 1
    out = GENERATED / OUTPUT
    on_disk = {p.name for p in out.glob("*")} if out.is_dir() else set()
    if args.check:
        stale = sorted(n for n, t in files.items() if not (out / n).exists() or (out / n).read_text() != t)
        stale += sorted(on_disk - set(files))
        if stale:
            print(f"merge_api: generated/{OUTPUT} is out of date: {', '.join(stale)}", file=sys.stderr)
            return 1
        print(f"generated/{OUTPUT} is up to date ({len(files)} files)")
        return 0
    out.mkdir(exist_ok=True)
    for name in on_disk - set(files):
        (out / name).unlink()
    for name, text in files.items():
        (out / name).write_text(text, newline="\n")
    print(f"generated/{OUTPUT}: {len(files)} files from {', '.join(args.modules)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
