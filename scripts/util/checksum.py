import hashlib
import csv
from pathlib import Path

class FileChecksumGenerator:
    def __init__(self):
        self.checksums = {}

    def load_checksums_from_csv(self, csv_path: str) -> None:
        with open(csv_path, "r", newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if len(row) != 2:
                    continue
                filepath, checksum = row
                self.checksums[filepath] = checksum

    def load_checksum_from_blob(self, blob : str) -> None:
        print(f"Loading checksum from blob: {blob}")
        for row in blob.split("\n"):
            print(row)
            parts = row.split(",")
            if len(parts) != 2:
                continue
            filepath, checksum = parts

            filepath = filepath.strip()
            checksum = checksum.strip()
            print(f"Loaded checksum for {filepath}: {checksum}")
            self.checksums[filepath] = checksum

    def __generate_file_checksum(self, filepath: str) -> str:
        hasher = hashlib.sha256()
        with open(filepath, "rb") as f:
            while chunk := f.read(8192):
                hasher.update(chunk)
        return hasher.hexdigest()

    def add_file(self, remote: str | Path, local: str | Path) -> None:
        self.checksums[remote] = self.__generate_file_checksum(local)

    def needs_update(self, remote: str, local: str | Path) -> bool:
        if remote not in self.checksums:
            return True
        current_checksum = self.__generate_file_checksum(local)

        # check if the current checksum matches the stored checksum (if we have one)
        if remote in self.checksums and self.checksums[remote] == current_checksum:
            return False

        return True
    
    def save_checksums_to_csv(self, csv_path: str) -> None:
        with open(csv_path, "w", newline='') as csvfile:
            writer = csv.writer(csvfile)
            for filepath, checksum in self.checksums.items():
                writer.writerow([filepath, checksum])