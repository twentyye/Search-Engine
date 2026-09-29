from __future__ import annotations

from pathlib import Path
from typing import Any

import cv2

from booksort.models import TextBlock

try:
    from paddleocr import PaddleOCR  # type: ignore
except Exception:  # pragma: no cover
    PaddleOCR = None  # type: ignore[assignment]


class OcrEngine:
    def __init__(self, lang: str = "ch", use_angle_cls: bool = True) -> None:
        if PaddleOCR is None:
            raise RuntimeError(
                "PaddleOCR is not available. Install dependencies with "
                "`pip install -r requirements-booksort.txt`."
            )
        candidates = [
            {"use_angle_cls": use_angle_cls, "lang": lang, "show_log": False},
            {"use_angle_cls": use_angle_cls, "lang": lang},
            {"lang": lang},
        ]
        last_error: Exception | None = None
        for kwargs in candidates:
            try:
                self._ocr = PaddleOCR(**kwargs)
                break
            except Exception as exc:  # pragma: no cover
                last_error = exc
        else:
            raise RuntimeError(f"Failed to initialize PaddleOCR: {last_error}")

    @staticmethod
    def preprocess(image_path: Path) -> Any:
        img = cv2.imread(str(image_path))
        if img is None:
            raise ValueError(f"Failed to read image: {image_path}")
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        denoise = cv2.GaussianBlur(gray, (3, 3), 0)
        return cv2.cvtColor(denoise, cv2.COLOR_GRAY2BGR)

    def recognize(self, image_path: Path) -> list[TextBlock]:
        image = self.preprocess(image_path)
        result = self._ocr.ocr(image, cls=True)
        blocks: list[TextBlock] = []
        if not result or not result[0]:
            return blocks
        for idx, item in enumerate(result[0]):
            bbox = item[0]
            text, conf = item[1]
            blocks.append(
                TextBlock(
                    text=text.strip(),
                    confidence=float(conf),
                    bbox=[[float(p[0]), float(p[1])] for p in bbox],
                    order=idx,
                )
            )
        return blocks

