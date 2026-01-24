from __future__ import annotations

from util.ssh import Remote, SSHConfig
from util.rpiignore import load_rpiignore

import argparse
import os
import posixpath
import shlex
import sys
from pathlib import Path
from typing import *


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "--engine",
        default=None,
        help="Override engine root (run `okay where` to get this).",
        required=True,
    )
    ap.add_argument("--host", default="nfr.local")
    ap.add_argument("--user", default="nfr")
    ap.add_argument("--password", default="formula")
    ap.add_argument("--port", type=int, default=22)
    ap.add_argument("--dash-dir", default="dash")
    ap.add_argument("--app", default="dash")
    ap.add_argument("--no-autoadd-hostkey", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument(
        "--clear", action="store_true", help="Clear remote directories before upload"
    )
    args = ap.parse_args()

    local_applications = Path(__file__).resolve().parent.parent
    local_app = local_applications / args.app
    if not local_app.is_dir():
        print(
            f"Error: Application directory does not exist: {local_app}",
            file=sys.stderr,
        )
        return 1

    remote_engine = f"{args.dash_dir}/engine"
    remote_apps = f"{args.dash_dir}/applications"
    remote_app = f"{remote_apps}/{args.app}"

    print(f"Target: {args.user}@{args.host}:{args.port}")
    print(f"Local engine: {args.engine}")
    print(f"Remote engine dir: {remote_engine}")
    print(f"Local app : {local_app}")
    print(f"Remote app dir : {remote_app}")

    rpiignore = load_rpiignore(local_applications)
    print(f".rpiignore rules for app payload: {len(rpiignore)} lines")


    cfg = SSHConfig(
        host=args.host,
        user=args.user,
        password=args.password if args.password else None,
        port=args.port,
        enable_dry_run=args.dry_run,
    )

    with Remote(cfg, accept_unknown_host_keys=not args.no_autoadd_hostkey) as r:
        try:
            if args.clear:
                print("==> Clearing remote dirs...")
                r.rm_rf(args.dash_dir)

            print("==> Ensuring remote dirs...")
            r.mkdir_p(remote_engine)
            r.mkdir_p(remote_apps)
            r.mkdir_p(remote_app)

            local_engine = Path(args.engine).resolve()
            print("==> Uploading engine files into dash/ ...")
            r.put_tree_with_rpiignore(local_engine, remote_engine, rpiignore)

            okay_install_script = posixpath.join(
                remote_engine, "okay", "scripts", "install.bash"
            )

            print("==> Ensuring install.bash is executable ...")
            r.allow_exec(okay_install_script)

            print("==> Ensuring install.bash has unix line endings ...")
            r.must(f"sed -i 's/\\r$//' {shlex.quote(okay_install_script)}")

            print("==> Running install.bash ...")
            r.must(okay_install_script)

            print("==> Uploading app payload (this script dir) ...")
            r.put_tree_with_rpiignore(local_app, remote_app, rpiignore)

            print("==> Running okay br ...")
            r.must(f"cd {shlex.quote(remote_app)} && okay br")

        except Exception as e:
            print(e, file=sys.stderr)
            return 1

    print("==> Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
