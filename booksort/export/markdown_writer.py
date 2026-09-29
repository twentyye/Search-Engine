from __future__ import annotations

from pathlib import Path

from booksort.models import BookIndex, PageResult


def write_markdown(book_id: str, pages: list[PageResult], index: BookIndex, target_path: Path) -> None:
    target_path.parent.mkdir(parents=True, exist_ok=True)
    lines: list[str] = [f"# {book_id}", ""]
    lines.append("## 目录索引")
    lines.append("")
    lines.append("### 主题分类")
    for topic, page_numbers in index.topics.items():
        joined = ", ".join(str(p) for p in sorted(set(page_numbers)))
        lines.append(f"- {topic}: {joined}")
    lines.append("")
    lines.append("### 图片索引")
    for item in index.images:
        lines.append(f"- 页 {item['page_number']}: {item['path']}")
    lines.append("")
    lines.append("---")
    lines.append("")
    lines.append("## 正文")
    lines.append("")
    for page in pages:
        lines.append(f"### Page {page.page_number}")
        lines.append("")
        lines.append(page.merged_text.strip() or "[Empty]")
        lines.append("")

    with target_path.open("w", encoding="utf-8") as f:
        f.write("\n".join(lines).strip() + "\n")

