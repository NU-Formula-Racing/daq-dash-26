from __future__ import annotations

import argparse
import os
import posixpath
import shlex
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional, Tuple
import paramiko


def run_local(cmd: List[str], cwd: Optional[Path] = None, *, shell: bool = False) -> str:
    try:
        if shell:
            out = subprocess.check_output(
                " ".join(cmd),
                cwd=str(cwd) if cwd else None,
                stderr=subprocess.STDOUT,
                shell=True,
            )
        else:
            out = subprocess.check_output(
                cmd,
                cwd=str(cwd) if cwd else None,
                stderr=subprocess.STDOUT,
            )
    except FileNotFoundError:
        raise
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {cmd}", file=sys.stderr)
        print(e.output.decode(errors="replace"), file=sys.stderr)
        raise
    return out.decode(errors="replace").strip()


def find_bash() -> Optional[Path]:
    """
    Find a bash executable to run `bash -lc ...`:
      - On macOS/Linux: /bin/bash
      - On Windows: Git Bash (Git for Windows)
    """
    if os.name != "nt":
        p = Path("/bin/bash")
        return p if p.exists() else None

    # Common Git for Windows locations:
    candidates = [
        Path(os.environ.get("PROGRAMFILES", r"C:\Program Files")) / "Git" / "bin" / "bash.exe",
        Path(os.environ.get("PROGRAMFILES", r"C:\Program Files")) / "Git" / "usr" / "bin" / "bash.exe",
        Path(os.environ.get("PROGRAMFILES(X86)", r"C:\Program Files (x86)")) / "Git" / "bin" / "bash.exe",
        Path(os.environ.get("PROGRAMFILES(X86)", r"C:\Program Files (x86)")) / "Git" / "usr" / "bin" / "bash.exe",
        # If user installed Git somewhere else but added to PATH:
        Path("bash.exe"),
    ]
    for c in candidates:
        try:
            if c.is_absolute() and c.exists():
                return c
            # "bash.exe" in PATH
            if str(c).lower() == "bash.exe":
                # Let CreateProcess search PATH
                return Path("bash.exe")
        except Exception:
            pass
    return None


def okay_where(engine_override: Optional[str]) -> Path:
    """
    Resolve engine root:
      - If --engine provided, use it.
      - Else try `okay where` directly.
      - If that fails (e.g., okay is a bash function), run `bash -lc "okay where"`.
    """
    if engine_override:
        p = Path(engine_override).expanduser().resolve()
        if not p.is_dir():
            raise RuntimeError(f"--engine path is not a directory: {p}")
        return p

    # Try direct (works if okay is a real executable on PATH)
    try:
        p = run_local(["okay", "where"])
        if p:
            path = Path(p).expanduser().resolve()
            if path.is_dir():
                return path
    except FileNotFoundError:
        pass

    # Fallback: run via bash login shell so .bashrc is sourced
    bash = find_bash()
    if not bash:
        raise RuntimeError(
            "Could not find bash to run `okay where`.\n"
            "Install Git for Windows (includes Git Bash), or pass --engine <path>."
        )

    # -l not strictly needed; -c is enough if user's .bashrc sets it up.
    # But many setups require interactive/login semantics; -l would read .bash_profile.
    # Git Bash uses ~/.bashrc with -i; however `bash -lc` usually works if okay is in shell init.
    cmd = [str(bash), "-lc", "okay where"]
    out = run_local(cmd, shell=False)
    if not out:
        raise RuntimeError("`bash -lc 'okay where'` returned empty.")
    path = Path(out).expanduser().resolve()
    if not path.is_dir():
        raise RuntimeError(f"`okay where` returned non-directory: {path}")
    return path


def git_file_list(engine_root: Path) -> List[str]:
    if not (engine_root / ".git").exists():
        raise RuntimeError(f"{engine_root} is not a git enginesitory (no .git).")

    out = run_local(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard"],
        cwd=engine_root,
    )
    return [line.strip() for line in out.splitlines() if line.strip()]


@dataclass
class SSHConfig:
    host: str
    user: str
    password: Optional[str]
    port: int = 22


class Remote:
    def __init__(self, cfg: SSHConfig, *, accept_unknown_host_keys: bool = True) -> None:
        self.cfg = cfg
        self.accept_unknown_host_keys = accept_unknown_host_keys
        self.client: Optional[paramiko.SSHClient] = None
        self.sftp: Optional[paramiko.SFTPClient] = None

    def __enter__(self) -> "Remote":
        self.client = paramiko.SSHClient()
        if self.accept_unknown_host_keys:
            self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        else:
            self.client.load_system_host_keys()

        self.client.connect(
            hostname=self.cfg.host,
            port=self.cfg.port,
            username=self.cfg.user,
            password=self.cfg.password,
            look_for_keys=True,
            allow_agent=True,
            timeout=15,
        )
        self.sftp = self.client.open_sftp()
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        try:
            if self.sftp:
                self.sftp.close()
        finally:
            if self.client:
                self.client.close()

    def run(self, cmd: str) -> Tuple[int, str, str]:
        assert self.client is not None
        full = f"bash -lc {shlex.quote(cmd)}"
        # get currrent directory of client to print
        dir = self.client.exec_command("pwd")
        print(f"[remote @ {dir[1].read().decode().strip()}]$ {full}")
        stdin, stdout, stderr = self.client.exec_command(full, get_pty=True)
        out = stdout.read().decode(errors="replace")
        err = stderr.read().decode(errors="replace")
        code = stdout.channel.recv_exit_status()
        return code, out, err

    def mkdir_p(self, remote_path: str) -> None:
        code, out, err = self.run(f"mkdir -p {shlex.quote(remote_path)}")
        if code != 0:
            raise RuntimeError(f"mkdir failed: {remote_path}\n{out}\n{err}")
        
    def rm_rf(self, remote_path: str) -> None:
        code, out, err = self.run(f"rm -rf {shlex.quote(remote_path)}")
        if code != 0:
            raise RuntimeError(f"rm -rf failed: {remote_path}\n{out}\n{err}")
        
    def allow_exec(self, remote_path: str) -> None:
        code, out, err = self.run(f"chmod +x {shlex.quote(remote_path)}")
        if code != 0:
            raise RuntimeError(f"chmod +x failed: {remote_path}\n{out}\n{err}")
        

    def put_file(self, local: Path, remote: str) -> None:
        assert self.sftp is not None
        parent = posixpath.dirname(remote.rstrip("/"))
        if parent:
            self.mkdir_p(parent)
        self.sftp.put(str(local), remote)

    def put_dir_all(self, local_dir: Path, remote_dir: str, *, exclude_names: Iterable[str] = ()) -> None:
        exclude = set(exclude_names)
        for root, dirs, files in os.walk(local_dir):
            root_path = Path(root)
            dirs[:] = [d for d in dirs if d not in exclude]

            rel = root_path.relative_to(local_dir)
            remote_root = remote_dir if str(rel) == "." else posixpath.join(remote_dir, rel.as_posix())

            self.mkdir_p(remote_root)

            for fn in files:
                if fn in exclude:
                    continue
                src = root_path / fn
                dst = posixpath.join(remote_root, fn)
                self.put_file(src, dst)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="nfr.local")
    ap.add_argument("--user", default="nfr")
    ap.add_argument("--password", default="formula")
    ap.add_argument("--port", type=int, default=22)
    ap.add_argument("--dash-dir", default="dash")
    ap.add_argument("--app", default="dash")
    ap.add_argument("--engine", default=None, help="Override engine root (skip `okay where`).")
    ap.add_argument("--no-autoadd-hostkey", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--clear", action="store_true", help="Clear remote directories before upload")
    args = ap.parse_args()

    engine_root = okay_where(args.engine)
    files = git_file_list(engine_root)
    script_dir = Path(__file__).resolve().parent

    remote_dash = f"{args.dash_dir}/engine"
    remote_apps = f"{args.dash_dir}/applications"
    remote_app = f"{remote_apps}/{args.app}"

    print(f"Target: {args.user}@{args.host}:{args.port}")
    print(f"Engine root: {engine_root}")
    print(f"Engine files to upload (git/.gitignore filtered): {len(files)}")
    print(f"Script dir (app payload): {script_dir}")
    print(f"Remote dash dir: {remote_dash}")
    print(f"Remote app dir : {remote_app}")

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
            for rel in files:
                local = engine_root / rel
                if local.is_file():
                    remote = posixpath.join(remote_dash, rel.replace("\\", "/"))
                    r.put_file(local, remote)


            okay_install_script = posixpath.join(remote_dash, "okay", "scripts", "install.bash")
            print("==> Ensuring install.bash is executable ...")
            r.allow_exec(okay_install_script)

            print("==> Running install.bash ...")
            code, out, err = r.run(okay_install_script)
            sys.stdout.write(out)
            if err.strip():
                sys.stderr.write(err)
            if code != 0:
                raise RuntimeError(f"install.bash failed ({code})")

            print("==> Uploading app payload (this script dir) ...")
            r.put_dir_all(script_dir, remote_app, exclude_names=(".git", ".DS_Store", "node_modules", "__pycache__"))

            print("==> Running okay br ...")
            code, out, err = r.run(f"cd {shlex.quote(remote_app)} && okay br")
            sys.stdout.write(out)
            if err.strip():
                sys.stderr.write(err)
            if code != 0:
                raise RuntimeError(f"okay br failed ({code})")
        except Exception as e:
            print(e, file=sys.stderr)
            return 1

    print("==> Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
