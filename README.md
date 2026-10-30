# NativeMobileDisplay

Mobile display driver and GPU scanout hand-off. Validates swaps, retires frames and drives the firmware framebuffer.

Current state: the CPU and GPU scanout paths work. The GOP scanout has no vblank interrupt; with `sheng-vsync=<Hz>` a work-loop timer sends the original family's vsync message to the compositor.

`offline-legacy-fb-wc=1` (default off) maps the scanout for the CPU copy write-combined (Normal non-cacheable on ARM) instead of uncached Device memory, and publishes each frame with `dsb sy` before the swap retires. It changes only CPU stores: the GPU hand-off (`sheng-fb-gpu=1`) and the scanout memory given to clients keep the uncached mapping. The kernel log prints `NativeLegacyDisplay: scanout mapping write-combined` when it is in effect.

Linked into XNU by `unixnu build kernel`.
