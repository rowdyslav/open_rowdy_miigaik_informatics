from pathlib import Path
from time import perf_counter

from matplotlib.backends.backend_agg import FigureCanvasAgg
from matplotlib.figure import Figure

from vector_divider import divide

SIZES = (10_000, 50_000, 100_000, 1_000_000, 10_000_000)


def main() -> None:
    cpp_times = []
    python_times = []

    print(f"{'Elements':>12} | {'C++ (s)':>12} | {'Python (s)':>12} | {'Speedup':>10}")
    print("-" * 57)

    for size in SIZES:
        vector = [float(index % 1000 + 1) for index in range(size)]

        cpp_result, cpp_time = divide(vector, 3.0)
        del cpp_result

        start = perf_counter()
        python_result = [value / 3.0 for value in vector]
        python_time = perf_counter() - start

        del python_result, vector
        cpp_times.append(cpp_time)
        python_times.append(python_time)

        speedup = f"{python_time / cpp_time:.2f}x" if cpp_time > 0.0 else "inf"
        print(f"{size:>12,} | {cpp_time:>12.6f} | {python_time:>12.6f} | {speedup:>10}")

    figure = Figure(figsize=(10, 6))
    canvas = FigureCanvasAgg(figure)
    axes = figure.subplots()
    axes.plot(SIZES, cpp_times, marker="o", label="C++ (Python/C API)")
    axes.plot(SIZES, python_times, marker="o", label="Python")
    axes.set_xscale("log")
    axes.set_xticks(SIZES, [f"{size:,}" for size in SIZES])
    axes.set_xlabel("Number of elements")
    axes.set_ylabel("Time, seconds")
    axes.set_title("Vector division: C++ vs Python")
    axes.grid(alpha=0.3)
    axes.legend()
    figure.tight_layout()

    output_path = Path(__file__).resolve().parents[2] / "benchmark_results.png"
    canvas.print_figure(output_path, dpi=300)
    print(f"\nGraph saved to {output_path}")


if __name__ == "__main__":
    main()
