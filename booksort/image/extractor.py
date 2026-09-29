from __future__ import annotations

from pathlib import Path

import cv2

from booksort.models import ImageRegion, TextBlock


def _bbox_to_rect(block: TextBlock) -> tuple[int, int, int, int]:
    xs = [p[0] for p in block.bbox]
    ys = [p[1] for p in block.bbox]
    x1, y1, x2, y2 = int(min(xs)), int(min(ys)), int(max(xs)), int(max(ys))
    return x1, y1, x2, y2


def extract_non_text_regions(
    image_path: Path, text_blocks: list[TextBlock], output_dir: Path, page_number: int
) -> list[ImageRegion]:
    image = cv2.imread(str(image_path))
    if image is None:
        return []
    h, w = image.shape[:2]
    mask = 255 * cv2.UMat(h, w, cv2.CV_8UC1).get()

    for block in text_blocks:
        x1, y1, x2, y2 = _bbox_to_rect(block)
        cv2.rectangle(mask, (x1, y1), (x2, y2), 0, thickness=-1)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    regions: list[ImageRegion] = []
    min_area = max(2000, int(w * h * 0.01))
    index = 1
    for contour in contours:
        x, y, cw, ch = cv2.boundingRect(contour)
        area = cw * ch
        if area < min_area:
            continue
        crop = image[y : y + ch, x : x + cw]
        out_name = f"page_{page_number:03d}_img_{index:02d}.png"
        out_path = output_dir / out_name
        cv2.imwrite(str(out_path), crop)
        regions.append(
            ImageRegion(path=str(out_path), bbox=[x, y, x + cw, y + ch], page_number=page_number)
        )
        index += 1
    return regions

