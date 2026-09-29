from __future__ import annotations

from pathlib import Path

import typer

from booksort.config import Paths
from booksort.pipeline import (
    build_pages,
    classify_from_pages,
    export_book,
    ingest_images,
    load_ingested_images,
    load_pages_from_json,
)

app = typer.Typer(help="Offline book page organizer CLI.")


def _workspace_root() -> Path:
    return Path.cwd()


@app.command()
def ingest(
    input: Path = typer.Option(..., exists=True, file_okay=False, dir_okay=True),
    book_id: str = typer.Option(..., help="Unique book id."),
) -> None:
    """Scan image folder and persist input list."""
    paths = Paths(_workspace_root(), book_id)
    images = ingest_images(input, paths)
    typer.echo(f"Ingested {len(images)} images for book `{book_id}`.")


@app.command()
def build(
    book_id: str = typer.Option(..., help="Unique book id."),
    lang: str = typer.Option("ch", help="PaddleOCR language code."),
) -> None:
    """Run OCR, parse layout, extract images, and build index."""
    paths = Paths(_workspace_root(), book_id)
    paths.ensure_dirs()
    images = load_ingested_images(paths)
    pages, index = build_pages(book_id=book_id, paths=paths, images=images, lang=lang)
    export_book(book_id, paths, pages, index, {"md", "epub", "json"})
    typer.echo(f"Build complete for `{book_id}`. Output: {paths.output_root}")


@app.command()
def classify(book_id: str = typer.Option(..., help="Unique book id.")) -> None:
    """Recompute index from existing page JSON files."""
    paths = Paths(_workspace_root(), book_id)
    pages = load_pages_from_json(paths)
    index = classify_from_pages(book_id, paths, pages)
    typer.echo(
        f"Classification complete for `{book_id}`. "
        f"Topics: {len(index.topics)}, images: {len(index.images)}"
    )


@app.command()
def export(
    book_id: str = typer.Option(..., help="Unique book id."),
    format: list[str] = typer.Option(
        [],
        "--format",
        help="One or more formats: md epub json",
    ),
    all: bool = typer.Option(False, "--all", help="Export md + epub + json."),
) -> None:
    """Export outputs based on existing page JSON + index."""
    requested = {"md", "epub", "json"} if all else set(f.lower() for f in format)
    if not requested:
        raise typer.BadParameter("Specify --all or at least one --format value.")

    paths = Paths(_workspace_root(), book_id)
    _ = load_ingested_images(paths)
    pages = load_pages_from_json(paths)
    index = classify_from_pages(book_id, paths, pages)
    export_book(book_id, paths, pages, index, requested)
    typer.echo(f"Export complete for `{book_id}`. Formats: {', '.join(sorted(requested))}")


if __name__ == "__main__":
    app()

