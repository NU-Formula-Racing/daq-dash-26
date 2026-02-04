from __future__ import annotations

from util.ssh import Remote, SSHConfig
from util.checksum import FileChecksumGenerator

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
    
    local_engine = Path(args.engine).resolve()

    if not local_engine.is_dir():
        print(f"Error: Engine directory does not exist: {args.engine}", file=sys.stderr)
        return 1

    remote_engine = f"{args.dash_dir}/engine"
    remote_apps = f"{args.dash_dir}/applications"
    remote_app = f"{remote_apps}/{args.app}"

    print(f"Target: {args.user}@{args.host}:{args.port}")
    print(f"Local engine: {local_engine}")
    print(f"Remote engine dir: {remote_engine}")
    print(f"Local app : {local_app}")
    print(f"Remote app dir : {remote_app}")

    rpiignore = local_applications.joinpath(".rpiignore")
    if rpiignore.is_file() is False:
        print(f"Error: .rpiignore file not found at {rpiignore}", file=sys.stderr)
        return 1

    cfg = SSHConfig(
        host=args.host,
        user=args.user,
        password=args.password if args.password else None,
        port=args.port,
        enable_dry_run=args.dry_run,
    )

    checksum_generator = FileChecksumGenerator()
    local_checksums_csv = "checksums.csv"
    remote_checksums_path = args.dash_dir + "/checksums.csv"

    def on_upload_file(local: Path, remote: Path) -> None:
        checksum_generator.add_file(remote, local)

    def filter_file(local: Path, remote : Path) -> bool:
        return checksum_generator.needs_update(remote, local)

    with Remote(cfg, accept_unknown_host_keys=not args.no_autoadd_hostkey) as r:
        try:
            if args.clear:
                print("==> Clearing remote dirs...")
                r.rm_rf(args.dash_dir)

            print("==> Ensuring remote dirs...")
            r.mkdir_p(remote_engine)
            r.mkdir_p(remote_apps)
            r.mkdir_p(remote_app)

            print("==> Checking remote checksums...")
            checksum_blob = r.read_remote_file(remote_checksums_path)
            if checksum_blob:
                checksum_generator.load_checksum_from_blob(checksum_blob)
            else:
                print("Remote checksums not found at", remote_checksums_path)


            print("==> Uploading engine files into dash/ ...")
            r.put_tree_with_rpiignore(local_engine, remote_engine, rpiignore, on_upload_file=on_upload_file, filter_func=filter_file)

            print("==> Uploading app payload (this script dir) ...")
            r.put_tree_with_rpiignore(local_app, remote_app, rpiignore, on_upload_file=on_upload_file, filter_func=filter_file)

            print("==> Uploading checksums.csv ...")
            checksum_generator.save_checksums_to_csv(local_checksums_csv)
            r.put_file(Path(local_checksums_csv), remote_checksums_path)

            okay_install_script = posixpath.join(
                remote_engine, "okay", "scripts", "install.bash"
            )

            print("==> Ensuring install.bash is executable ...")
            r.allow_exec(okay_install_script)

            print("==> Ensuring install.bash has unix line endings ...")
            r.must(f"sed -i 's/\\r$//' {shlex.quote(okay_install_script)}")

            print("==> Running install.bash ...")
            r.must(okay_install_script)

            print("==> Running okay br ...")
            r.must(
                f"source ~/.bashrc && "
                f"cd {shlex.quote(remote_app)} && "
                f"okay init && okay br --target rpi"
            )



        except Exception as e:
            print(e, file=sys.stderr)
            return 1

    print("==> Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
