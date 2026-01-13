from __future__ import annotations

from util.ssh import Remote, SSHConfig

import argparse
import os
import posixpath
import shlex
import sys
from pathlib import Path
from typing import *

import fnmatch

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


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--engine", default=None, help="Override engine root (run `okay where` to get this).", required=True)
    ap.add_argument("--host", default="nfr.local")
    ap.add_argument("--user", default="nfr")
    ap.add_argument("--password", default="formula")
    ap.add_argument("--port", type=int, default=22)
    ap.add_argument("--dash-dir", default="dash")
    ap.add_argument("--app", default="dash")
    ap.add_argument("--no-autoadd-hostkey", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--clear", action="store_true", help="Clear remote directories before upload")
    args = ap.parse_args()

    script_dir = Path(__file__).resolve().parent

    remote_dash = f"{args.dash_dir}/engine"
    remote_apps = f"{args.dash_dir}/applications"
    remote_app = f"{remote_apps}/{args.app}"

    print(f"Target: {args.user}@{args.host}:{args.port}")
    print(f"Engine root: {args.engine}")
    print(f"Script dir (app payload): {script_dir}")
    print(f"Remote dash dir: {remote_dash}")
    print(f"Remote app dir : {remote_app}")

    rpiignore = load_rpiignore(script_dir)
    print(f".rpiignore rules for app payload: {len(rpiignore)} lines")

    if args.dry_run:
        print("[dry-run] Skipping uploads/remote commands.")
        return 0

    cfg = SSHConfig(
        host=args.host,
        user=args.user,
        password=args.password if args.password else None,
        port=args.port,
    )

    with Remote(cfg, accept_unknown_host_keys=not args.no_autoadd_hostkey) as r:
        try:
            if args.clear:
                print("==> Clearing remote dirs...")
                r.rm_rf(args.dash_dir)

            print("==> Ensuring remote dirs...")
            r.mkdir_p(remote_dash)
            r.mkdir_p(remote_apps)
            r.mkdir_p(remote_app)

            print("==> Uploading engine files into dash/ ...")
            r.put_tree_with_rpiignore(args.args.engine, remote_dash, rpiignore)

            okay_install_script = posixpath.join(remote_dash, "okay", "scripts", "install.bash")
            print("==> Ensuring install.bash is executable ...")
            r.allow_exec(okay_install_script)

            print("==> Ensuring install.bash has unix line endings ...")
            r.must(f"sed -i 's/\\r$//' {shlex.quote(okay_install_script)}")

            print("==> Running install.bash ...")
            r.must(okay_install_script)

            print("==> Uploading app payload (this script dir) ...")
            r.put_tree_with_rpiignore(script_dir, remote_app, rpiignore)

            print("==> Running okay br ...")
            r.must(f"cd {shlex.quote(remote_app)} && okay br")

        except Exception as e:
            print(e, file=sys.stderr)
            return 1

    print("==> Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
