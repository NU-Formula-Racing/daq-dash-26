from __future__ import annotations

from pathlib import Path
import fnmatch
from typing import *
import os
from gitignore_parser import parse_gitignore



def get_files_with_rpiignore(root: Path, rpiiignore_file : Path | str) -> List[Tuple[Path, str]]:
    res = []
    root = root.resolve()
    matches = parse_gitignore(rpiiignore_file)

    for dirpath, dirnames, filenames in os.walk(root):
        for filename in filenames:
            rel_posix = os.path.relpath(dirpath, root)
            rel_posix = os.path.join(rel_posix, filename)
            if matches(rel_posix):
                continue
            
            res.append((Path(dirpath) / filename, rel_posix))


    return res