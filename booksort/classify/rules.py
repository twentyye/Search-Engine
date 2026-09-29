from __future__ import annotations

from collections import defaultdict

from booksort.models import BookIndex, PageResult


CHAPTER_HINTS = ("第", "chapter", "chap.", "章节", "unit")
TOPIC_KEYWORDS = {
    "数学": ["公式", "定理", "证明", "函数", "math"],
    "历史": ["朝代", "年代", "战争", "history"],
    "计算机": ["算法", "编程", "代码", "computer", "program"],
    "物理": ["力", "能量", "运动", "physics"],
}


def build_index(book_id: str, pages: list[PageResult]) -> BookIndex:
    chapter_map: dict[str, list[int]] = defaultdict(list)
    topic_map: dict[str, list[int]] = defaultdict(list)
    image_index: list[dict[str, object]] = []

    for page in pages:
        text = page.merged_text.lower()

        for hint in CHAPTER_HINTS:
            if hint.lower() in text:
                chapter_map["likely_chapter_pages"].append(page.page_number)
                break

        for topic, keywords in TOPIC_KEYWORDS.items():
            if any(keyword.lower() in text for keyword in keywords):
                topic_map[topic].append(page.page_number)

        for region in page.images:
            image_index.append(
                {
                    "page_number": page.page_number,
                    "path": region.path,
                    "bbox": region.bbox,
                }
            )

    return BookIndex(
        book_id=book_id,
        chapters=dict(chapter_map),
        topics=dict(topic_map),
        images=image_index,
    )

