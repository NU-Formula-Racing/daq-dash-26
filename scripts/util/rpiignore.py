from __future__ import annotations

from pathlib import Path
import fnmatch
from typing import *
import os

def _to_posix_rel(path: Path, root: Path) -> str:
    rel = path.relative_to(root).as_posix()
    return rel

def load_rpiignore(root: Path, filename: str = ".rpiignore") -> List[str]:
    p = root / filename
    if not p.exists():
        return []
    lines: List[str] = []
    for raw in p.read_text(encoding="utf-8", errors="replace").splitlines():
        s = raw.strip()
        if not s or s.startswith("#"):
            continue
        lines.append(s)
    return lines

def _match_pattern(rel_posix: str, pattern: str) -> bool:
    anchored = pattern.startswith("/")
    if anchored:
        pattern = pattern[1:]

    dir_only = pattern.endswith("/")
    if dir_only:
        pattern = pattern[:-1]
        if rel_posix == pattern or rel_posix.startswith(pattern + "/"):
            return True
        return False

    candidates = [rel_posix]
    if not anchored:
        parts = rel_posix.split("/")
        for i in range(1, len(parts)):
            candidates.append("/".join(parts[i:]))

    for c in candidates:
        if fnmatch.fnmatchcase(c, pattern):
            return True

    return False

def should_include(rel_posix: str, rules: List[str]) -> bool:
    include = True
    for rule in rules:
        neg = rule.startswith("!")
        pat = rule[1:] if neg else rule
        if _match_pattern(rel_posix, pat):
            include = True if neg else False
    return include

def iter_files_with_rpiignore(root: Path, rules: List[str]) -> Iterable[Tuple[Path, str]]:
    root = root.resolve()
    for dirpath, dirnames, filenames in os.walk(root):
        dpath = Path(dirpath)

        pruned: List[str] = []
        for dn in list(dirnames):
            rel_dir = _to_posix_rel(dpath / dn, root)
            if not should_include(rel_dir, rules) and not should_include(rel_dir + "/__keep__", rules):
                pruned.append(dn)

        for dn in pruned:
            dirnames.remove(dn)

        for fn in filenames:
            local = dpath / fn
            rel = _to_posix_rel(local, root)
            if should_include(rel, rules):
                yield local, rel