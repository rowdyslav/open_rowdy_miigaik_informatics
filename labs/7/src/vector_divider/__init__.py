from ._core import divide as _divide

__all__ = ["divide"]


def divide(
    vector: list[float | int], divisor: float | int
) -> tuple[list[float], float]:
    return _divide(vector, divisor)
