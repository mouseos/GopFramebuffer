# NativeMobileDisplay

Mobile display driver and GPU scanout hand-off. Validates swaps, retires frames and drives the firmware framebuffer.

Current state: the CPU and GPU scanout paths work. The GOP scanout has no vblank interrupt; with `sheng-vsync=<Hz>` a work-loop timer sends the original family's vsync message to the compositor.

`offline-legacy-fb-wc=1` (default off) maps the scanout for the CPU copy write-combined (Normal non-cacheable on ARM) instead of uncached Device memory, and publishes each frame with `dsb sy` before the swap retires. It changes only kernel CPU stores. With `sheng-fb-gpu=1` the GPU writes the frames and only a fallback CPU copy (after the GPU reported or lost its copy) goes through the write-combined mapping; the scanout memory given to clients, including the GPU's import, keeps the uncached mapping. The mapping is safe only while no cacheable alias of the scanout exists (it lies outside the kernel's DRAM physmap). The kernel log prints `NativeLegacyDisplay: scanout mapping write-combined` when it is in effect.

Linked into XNU by `unixnu build kernel`.
