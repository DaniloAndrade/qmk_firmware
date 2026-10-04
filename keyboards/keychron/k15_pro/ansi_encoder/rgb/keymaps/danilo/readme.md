# danilo keymap

Home-row-mods-tuned variant of the `via` keymap for the Keychron K15 Pro RGB.

Functionally identical to `via` (same layers, same `encoder_map`, same
`VIA_ENABLE`/`ENCODER_MAP_ENABLE`) — home row mods are still configured
dynamically via the Keychron Launch app, not hard-coded here. The only
difference is `config.h`, which enables `FLOW_TAP_TERM`, `PERMISSIVE_HOLD`,
and `CHORDAL_HOLD` to prevent accidental mod-tap activation during fast
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
