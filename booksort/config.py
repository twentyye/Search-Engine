from __future__ import annotations

from pathlib import Path


SUPPORTED_EXTENSIONS = {".jpg", ".jpeg", ".png", ".webp", ".bmp", ".tif", ".tiff"}


class Paths:
    def __init__(self, workspace_root: Path, book_id: str) -> None:
        self.workspace_root = workspace_root
        self.output_root = workspace_root / "output" / "books" / book_id
        self.pages_dir = self.output_root / "pages"
        self.images_dir = self.output_root / "images"
        self.cache_dir = self.output_root / ".cache"
        self.raw_list_file = self.cache_dir / "input_images.txt"
        self.readable_md = self.output_root / "readable.md"
        self.readable_epub = self.output_root / "readable.epub"
        self.index_json = self.output_root / "index.json"

    def ensure_dirs(self) -> None:
        self.output_root.mkdir(parents=True, exist_ok=True)
        self.pages_dir.mkdir(parents=True, exist_ok=True)
        self.images_dir.mkdir(parents=True, exist_ok=True)
        self.cache_dir.mkdir(parents=True, exist_ok=True)

