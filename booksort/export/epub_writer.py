from __future__ import annotations

from pathlib import Path

from ebooklib import epub

from booksort.models import BookIndex, PageResult


def _build_nav_xhtml(index: BookIndex) -> str:
    items = "".join(
        f"<li>{topic}: {', '.join(str(p) for p in sorted(set(pages)))}</li>"
        for topic, pages in index.topics.items()
    )
    return f"""
    <html><body>
    <h2>Topic Index</h2>
    <ul>{items}</ul>
    </body></html>
    """


def write_epub(book_id: str, pages: list[PageResult], index: BookIndex, target_path: Path) -> None:
    target_path.parent.mkdir(parents=True, exist_ok=True)
    book = epub.EpubBook()
    book.set_identifier(book_id)
    book.set_title(book_id)
    book.set_language("zh")
    book.add_author("booksort")

    nav = epub.EpubHtml(title="Index", file_name="index.xhtml", lang="zh")
    nav.content = _build_nav_xhtml(index)
    book.add_item(nav)

    spine = [nav]
    toc: list[epub.EpubHtml] = [nav]
    for page in pages:
        chapter = epub.EpubHtml(
            title=f"Page {page.page_number}",
            file_name=f"page_{page.page_number:03d}.xhtml",
            lang="zh",
        )
        body = page.merged_text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        chapter.content = f"<html><body><h2>Page {page.page_number}</h2><pre>{body}</pre></body></html>"
        book.add_item(chapter)
        spine.append(chapter)
        toc.append(chapter)

    book.toc = tuple(toc)
    book.spine = spine
    book.add_item(epub.EpubNcx())
    book.add_item(epub.EpubNav())

    epub.write_epub(str(target_path), book, {})

