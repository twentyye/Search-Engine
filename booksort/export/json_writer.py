from __future__ import annotations

import json
from pathlib import Path

from booksort.models import BookIndex, PageResult


def write_page_json(page: PageResult, target_path: Path) -> None:
    target_path.parent.mkdir(parents=True, exist_ok=True)
    with target_path.open("w", encoding="utf-8") as f:
        json.dump(page.to_dict(), f, ensure_ascii=False, indent=2)


def write_index_json(index: BookIndex, target_path: Path) -> None:
    target_path.parent.mkdir(parents=True, exist_ok=True)
    with target_path.open("w", encoding="utf-8") as f:
        json.dump(index.to_dict(), f, ensure_ascii=False, indent=2)

