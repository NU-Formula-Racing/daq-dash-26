from __future__ import annotations

from util.rpiignore import get_files_with_rpiignore

from pathlib import Path
from typing import *
import paramiko
from dataclasses import dataclass
import posixpath
import shlex
import sys
from tqdm import tqdm

@dataclass
class SSHConfig:
    host: str
    user: str
    password: Optional[str]
    port: int = 22
    enable_dry_run: bool = False # if True, don't actually execute commands


class Remote:
    def __init__(
        self, cfg: SSHConfig, *, accept_unknown_host_keys: bool = True
    ) -> None:
        self.cfg = cfg
        self.accept_unknown_host_keys = accept_unknown_host_keys
        self.client: Optional[paramiko.SSHClient] = None
        self.sftp: Optional[paramiko.SFTPClient] = None

    def __enter__(self) -> "Remote":
        if self.cfg.enable_dry_run:
            print("[dry-run] Skipping SSH connection.")
            return self

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
        if self.cfg.enable_dry_run:
            return

        try:
            if self.sftp:
                self.sftp.close()
        finally:
            if self.client:
                self.client.close()

    def run(self, cmd: str, verbose=True) -> Tuple[int, str, str]:
        if self.cfg.enable_dry_run:
            print(f"[dry-run] SSH command: {cmd}")
            return 0, "", ""

        assert self.client is not None, "SSH client is not initialized."

        full = f"bash -lic {shlex.quote(cmd)}"
        # get currrent directory of client to print
        if verbose:
            dir = self.client.exec_command("pwd")
            print(f"[remote @ {dir[1].read().decode().strip()}]$ {full}")
        stdin, stdout, stderr = self.client.exec_command(full, get_pty=True)
        out = stdout.read().decode(errors="replace")
        err = stderr.read().decode(errors="replace")
        code = stdout.channel.recv_exit_status()
        return code, out, err

    def must(self, cmd: str, verbose=True) -> None:
        if self.cfg.enable_dry_run:
            print(f"[dry-run] SSH command must: {cmd}")
            return
        
        code, out, err = self.run(cmd, verbose=verbose)
        if out.strip():
            sys.stdout.write(out)
        if err.strip():
            sys.stderr.write(err)

        if code != 0:
            raise RuntimeError(f"Command failed: {cmd}\n{out}\n{err}")

    def mkdir_p(self, remote_path: str, verbose=True) -> None:
        self.must(f"mkdir -p {shlex.quote(remote_path)}", verbose=verbose)

    def rm_rf(self, remote_path: str, verbose=True) -> None:
        self.must(f"rm -rf {shlex.quote(remote_path)}", verbose=True)

    def allow_exec(self, remote_path: str, verbose=True) -> None:
        self.must(f"chmod +x {shlex.quote(remote_path)}", verbose=verbose)

    def put_file(self, local: Path, remote: str) -> None:
        if self.cfg.enable_dry_run:
            return

        assert self.sftp is not None, "SFTP client is not initialized."

        if not local.exists():
            raise RuntimeError(f"Local file does not exist: {local}")
        
        remote = remote.replace("\\", "/")  # normalize to POSIX

        # ensure remote directory exists
        remote_dir = posixpath.dirname(remote)
        self.mkdir_p(remote_dir, verbose=False)
        self.sftp.put(str(local), remote)

    def read_remote_file(self, remote: str) -> str:
        if self.cfg.enable_dry_run:
            print(f"[dry-run] Reading remote file: {remote}")
            return None

        assert self.sftp is not None, "SFTP client is not initialized."

        # check that the remote file exists
        try:
            with self.sftp.open(remote, "r") as f:
                return f.read().decode()
        except FileNotFoundError:
            return None


    def put_tree_with_rpiignore(
        self, local_root: Path, remote_root: str, rules: List[str], on_upload_file: Optional[Callable[[Path, Path, str], None]] = None, filter_func: Optional[Callable[[Path], bool]] = None
    ) -> None:
        files = get_files_with_rpiignore(local_root, rules)
        for local, rel in tqdm(files, desc="Uploading files", unit="file"):
            # print the relative path being uploaded, but in a way that works with tqdm
            rel_posix = rel.as_posix() if isinstance(rel, Path) else str(rel).replace("\\", "/")
            remote = posixpath.join(remote_root, rel_posix)

            remote = posixpath.join(remote_root, rel)
            if filter_func and not filter_func(local, remote):
                # tqdm.write(f"Skipping: {local} -> {remote_root}/{rel}")
                continue

            tqdm.write(f"Uploading: {local} -> {remote_root}/{rel}")
            self.put_file(local, f"{remote_root}/{rel}")
            if on_upload_file:
                on_upload_file(local, remote)
