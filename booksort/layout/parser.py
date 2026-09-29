from __future__ import annotations

from booksort.models import TextBlock


def _block_key(block: TextBlock) -> tuple[float, float]:
    xs = [p[0] for p in block.bbox]
    ys = [p[1] for p in block.bbox]
    return min(ys), min(xs)


def sort_blocks_for_reading(blocks: list[TextBlock]) -> list[TextBlock]:
    sorted_blocks = sorted(blocks, key=_block_key)
    for idx, block in enumerate(sorted_blocks):
        block.order = idx
    return sorted_blocks


def merge_blocks_to_text(blocks: list[TextBlock]) -> str:
    if not blocks:
        return ""
    lines: list[str] = []
    buffer: list[str] = []
    previous_y = None
    threshold = 24.0

    for block in blocks:
        y = min(p[1] for p in block.bbox)
        txt = block.text.strip()
        if not txt:
            continue
        if previous_y is None:
            buffer.append(txt)
        elif abs(y - previous_y) <= threshold:
            buffer.append(txt)
        else:
            lines.append(" ".join(buffer))
            buffer = [txt]
        previous_y = y

    if buffer:
        lines.append(" ".join(buffer))

    # Basic cleanup for OCR line-break artifacts.
    cleaned = "\n".join(line.replace("  ", " ").strip() for line in lines if line.strip())
    return cleaned

