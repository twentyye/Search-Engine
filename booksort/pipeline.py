from __future__ import annotations

from pathlib import Path
import json

from booksort.classify.rules import build_index
from booksort.config import Paths, SUPPORTED_EXTENSIONS
from booksort.export.epub_writer import write_epub
from booksort.export.json_writer import write_index_json, write_page_json
from booksort.export.markdown_writer import write_markdown
from booksort.image.extractor import extract_non_text_regions
from booksort.layout.parser import merge_blocks_to_text, sort_blocks_for_reading
from booksort.models import BookIndex, ImageRegion, PageResult, TextBlock, sort_images_for_processing
from booksort.ocr.engine import OcrEngine


def collect_images(input_dir: Path) -> list[Path]:
    images = [p for p in input_dir.rglob("*") if p.suffix.lower() in SUPPORTED_EXTENSIONS]
    return sort_images_for_processing(images)


def ingest_images(input_dir: Path, paths: Paths) -> list[Path]:
    images = collect_images(input_dir)
    if not images:
        raise ValueError(f"No supported images found in: {input_dir}")
    paths.ensure_dirs()
    with paths.raw_list_file.open("w", encoding="utf-8") as f:
        for img in images:
            f.write(str(img.resolve()) + "\n")
    return images


def load_ingested_images(paths: Paths) -> list[Path]:
    if not paths.raw_list_file.exists():
        raise FileNotFoundError("No ingested images list found, run ingest first.")
    lines = paths.raw_list_file.read_text(encoding="utf-8").splitlines()
    images = [Path(line.strip()) for line in lines if line.strip()]
    if not images:
        raise ValueError("Ingested image list is empty.")
    return images


def build_pages(book_id: str, paths: Paths, images: list[Path], lang: str = "ch") -> tuple[list[PageResult], BookIndex]:
    _ = book_id
    engine = OcrEngine(lang=lang)
    pages: list[PageResult] = []
    for idx, image_path in enumerate(images, start=1):
        text_blocks = sort_blocks_for_reading(engine.recognize(image_path))
        merged_text = merge_blocks_to_text(text_blocks)
        image_regions = extract_non_text_regions(image_path, text_blocks, paths.images_dir, idx)
        page = PageResult(
            page_number=idx,
            source_image=str(image_path),
            text_blocks=text_blocks,
            images=image_regions,
            merged_text=merged_text,
        )
        write_page_json(page, paths.pages_dir / f"page_{idx:03d}.json")
        pages.append(page)

    index = build_index(book_id, pages)
    write_index_json(index, paths.index_json)
    return pages, index


def load_pages_from_json(paths: Paths) -> list[PageResult]:
    page_files = sorted(paths.pages_dir.glob("page_*.json"))
    if not page_files:
        raise FileNotFoundError("No page JSON found, run build first.")
    pages: list[PageResult] = []
    for page_file in page_files:
        raw = json.loads(page_file.read_text(encoding="utf-8"))
        text_blocks = [
            TextBlock(
                text=tb["text"],
                confidence=float(tb["confidence"]),
                bbox=[[float(p[0]), float(p[1])] for p in tb["bbox"]],
                order=int(tb.get("order", 0)),
            )
            for tb in raw.get("text_blocks", [])
        ]
        images = [
            ImageRegion(
                path=img["path"],
                bbox=[int(v) for v in img["bbox"]],
                page_number=int(img["page_number"]),
            )
            for img in raw.get("images", [])
        ]
        pages.append(
            PageResult(
                page_number=int(raw["page_number"]),
                source_image=raw["source_image"],
                text_blocks=text_blocks,
                images=images,
                merged_text=raw.get("merged_text", ""),
            )
        )
    return pages


def classify_from_pages(book_id: str, paths: Paths, pages: list[PageResult]) -> BookIndex:
    index = build_index(book_id, pages)
    write_index_json(index, paths.index_json)
    return index


def export_book(book_id: str, paths: Paths, pages: list[PageResult], index: BookIndex, formats: set[str]) -> None:
    if "md" in formats:
        write_markdown(book_id, pages, index, paths.readable_md)
    if "epub" in formats:
        write_epub(book_id, pages, index, paths.readable_epub)
    if "json" in formats:
        write_index_json(index, paths.index_json)

