from __future__ import annotations

from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Any


@dataclass
class TextBlock:
    text: str
    confidence: float
    bbox: list[list[float]]
    order: int = 0


@dataclass
class ImageRegion:
    path: str
    bbox: list[int]
    page_number: int


@dataclass
class PageResult:
    page_number: int
    source_image: str
    text_blocks: list[TextBlock] = field(default_factory=list)
    images: list[ImageRegion] = field(default_factory=list)
    merged_text: str = ""

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class BookIndex:
    book_id: str
    chapters: dict[str, list[int]] = field(default_factory=dict)
    topics: dict[str, list[int]] = field(default_factory=dict)
    images: list[dict[str, Any]] = field(default_factory=list)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


def sort_images_for_processing(image_paths: list[Path]) -> list[Path]:
    def key_func(path: Path) -> tuple[int, str]:
        stem = path.stem
        digits = "".join(ch for ch in stem if ch.isdigit())
        num = int(digits) if digits else 10**9
        return num, path.name.lower()

    return sorted(image_paths, key=key_func)

