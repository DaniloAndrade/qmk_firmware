# danilo keymap

Home-row-mods-tuned variant of the `via` keymap for the Keychron K15 Pro RGB.

Same layers, `encoder_map` and `VIA_ENABLE`/`ENCODER_MAP_ENABLE` as `via` —
home row mods are still configured dynamically via the Keychron Launch app,
not hard-coded here. The differences are the combos (see below) and
`config.h`, which enables `FLOW_TAP_TERM`, `PERMISSIVE_HOLD`, and
`CHORDAL_HOLD` (`FLOW_TAP_TERM` and `CHORDAL_HOLD` need the core backport
described below) to prevent accidental mod-tap activation during fast
typing (e.g. GUI+Space opening Spotlight while typing "a " quickly), and
raises `TAPPING_TERM` to 250ms so a slower finger (e.g. pinky) holding a
key slightly longer than the 200ms default doesn't resolve to "hold" by
plain timeout — while keeping deliberate held combos working.

See `docs/superpowers/specs/2026-07-10-k15-pro-homerow-mods-design.md` and
`docs/superpowers/plans/2026-07-10-k15-pro-homerow-mods.md` for the full
rationale and design.

## Combos

Three combos, matching the K3 Ultra ZMK keymap: `U+I` is Backspace (18 ms
window), `Z+X` is `[` and `,+.` is `]` (30 ms). They are disabled on the Fn
layers and only fire after a 150 ms pause since the previous key press
(`combo_should_trigger` in `keymap.c`; QMK has no `require-prior-idle`).

Home-row combos (parentheses, braces) are left out on purpose: QMK matches
combos by keycode, so a Launcher `MT()` on D/F/J/K/S/L would keep a plain
`KC_D`+`KC_F` combo from ever matching.

## Core backport (Chordal Hold and Flow Tap)

The K15 Pro lives on the `wireless_playground` lineage, whose QMK core predates
Chordal Hold and Flow Tap (upstream `quantum/action_tapping.c` has no
`CHORDAL_HOLD` or `FLOW_TAP`), so on that core `CHORDAL_HOLD`, `FLOW_TAP_TERM`
and `chordal_hold_layout` were silently ignored. The `k15-pro-chordal-hold`
branch carries a backport of the `quantum/` part of these upstream QMK commits,
applied in this order: `544ddde1136` (Chordal Hold), `9d799aff971`,
`3ab2b3b6e2a`, `b69bf4b885c`, `8d8dcb089ed` (Flow Tap), `73e2ef486ab`,
`c26449e64f1`. The docs, `data/` and `keyboard_c.py` parts of those commits do
not apply on this core and are not needed, because this keymap defines
`chordal_hold_layout` itself. The matching upstream tests are under
`tests/tap_hold_configurations`.

Run them on macOS with
`make test:tap_hold_configurations EXTRAFLAGS="-Wno-include-next-absolute-path -Wno-c++11-narrowing"`;
the existing test harness also needs a temporary `#include <sstream>` at the top
of `tests/test_common/keycode_util.cpp` with current clang (do not commit it).
