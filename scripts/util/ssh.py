from __future__ import annotations

from util.rpiignore import iter_files_with_rpiignore

from pathlib import Path
from typing import *
import paramiko
from dataclasses import dataclass
import posixpath
import shlex
import sys

@dataclass
class SSHConfig:
    host: str
    user: str
    password: Optional[str]
    port: int = 22


class Remote:
    def __init__(
        self, cfg: SSHConfig, *, accept_unknown_host_keys: bool = True
    ) -> None:
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

    def run(self, cmd: str, verbose=True) -> Tuple[int, str, str]:
        assert self.client is not None
        full = f"bash -lc {shlex.quote(cmd)}"
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
        assert self.sftp is not None
        print(f"[upload] {local} -> {remote}")

        if not local.exists():
            raise RuntimeError(f"Local file does not exist: {local}")

        # ensure remote directory exists
        remote_dir = posixpath.dirname(remote)
        self.mkdir_p(remote_dir, verbose=False)

        self.sftp.put(str(local), remote)

    def put_tree_with_rpiignore(
        self, local_root: Path, remote_root: str, rules: List[str]
    ) -> None:
        for local, rel in iter_files_with_rpiignore(local_root, rules):
            remote = posixpath.join(remote_root, rel)
            self.put_file(local, remote)
