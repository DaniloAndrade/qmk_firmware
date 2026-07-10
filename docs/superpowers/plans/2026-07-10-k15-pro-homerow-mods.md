# K15 Pro RGB — Home Row Mods Tap-Hold Fix Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create a new QMK keymap for the Keychron K15 Pro RGB (`keymaps/danilo`) that mitigates accidental mod-tap activation during fast typing (e.g. GUI+Space opening macOS Spotlight while typing `a ` quickly), while staying fully compatible with the Keychron Launch (VIA) app and without modifying any existing file.

**Architecture:** New keymap directory `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/` starts as a byte-for-byte copy of the existing `via` keymap (so the Keychron Launch app keeps working — home row mods stay dynamically configured in the keyboard's EEPROM, not hard-coded here). A `config.h` adds three QMK tap-hold behaviors (`FLOW_TAP_TERM`, `PERMISSIVE_HOLD`, `CHORDAL_HOLD`), and `keymap.c` gains a `chordal_hold_layout` table that tells QMK which physical hand presses each key, so accidental same-hand/fast-typing chords resolve as taps while deliberate opposite-hand or held combos still resolve as holds.

**Tech Stack:** QMK Firmware (C, STM32L432, `make`-based build, `stm32-dfu` bootloader). No unit test framework exists for keymaps — verification is: (1) the firmware compiles cleanly, and (2) manual on-hardware testing per a fixed checklist. Every task's "test" step is therefore a compile, and the final task is the manual hardware checklist.

## Global Constraints

- Do not modify any existing file in the repository, especially `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/via/` — the new keymap is fully additive, in its own `keymaps/danilo/` directory.
- The resulting firmware must keep working with the Keychron Launch app (VIA protocol) for remapping keys — `VIA_ENABLE = yes` and `ENCODER_MAP_ENABLE = yes` must stay set, and no hard-coded `MT()`/home row mod keycodes go into `keymap.c` (those stay configured dynamically via Launch, in EEPROM).
- The deliberate GUI+Space combo (holding `A` past the tapping term, then pressing Space) must keep working — the fix targets fast "rolling" only, not the combo itself.
- Target build: `keychron/k15_pro/ansi_encoder/rgb`, keymap name `danilo`.

---

## Task 1: Verify the build environment and get a baseline compile

**Files:**
- None created or modified — this task only verifies tooling and produces a baseline binary for comparison.

**Interfaces:**
- Consumes: nothing.
- Produces: a working ARM/QMK toolchain, and a baseline binary at `.build/keychron_k15_pro_ansi_encoder_rgb_via.bin` used informally to sanity-check later tasks didn't break anything the `via` keymap already had working.

- [ ] **Step 1: Check whether the ARM toolchain is already installed**

Run: `which arm-none-eabi-gcc dfu-util`
Expected: if both print a path, skip to Step 3. If either prints nothing (not found), continue to Step 2.

- [ ] **Step 2: Install the build environment**

Run (from the repo root):
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
util/qmk_install.sh
```
Expected: script detects macOS, installs the missing Homebrew packages (ARM toolchain, `dfu-util`, `avrdude`, etc.) and exits 0. This can take several minutes on first run.

- [ ] **Step 3: Compile the existing `via` keymap as a baseline**

Run:
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
make keychron/k15_pro/ansi_encoder/rgb:via
```
Expected: build completes with no `ERROR` lines, ending in something like `Copying keychron_k15_pro_ansi_encoder_rgb_via.bin to qmk_firmware folder` and exit code 0. Confirm the file exists:
```bash
ls -la keychron_k15_pro_ansi_encoder_rgb_via.bin
```
Expected: file listed with a non-zero size (typically 60-120 KB for this MCU).

No commit for this task — no repository files changed.

---

## Task 2: Create the `danilo` keymap as an exact copy of `via`

**Files:**
- Create: `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c`
- Create: `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/rules.mk`

**Interfaces:**
- Consumes: nothing from other tasks.
- Produces: a `danilo` keymap functionally identical to `via` (same four layers `MAC_BASE`/`MAC_FN`/`WIN_BASE`/`WIN_FN`, same `encoder_map`, `VIA_ENABLE`/`ENCODER_MAP_ENABLE` both `yes`). Task 3 and Task 4 add files/edits on top of this.

- [ ] **Step 1: Create the keymap directory**

Run:
```bash
mkdir -p /Users/danilosandrade/workspace/qmk_firmware/keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo
```

- [ ] **Step 2: Create `rules.mk` as a copy of the `via` one**

Create `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/rules.mk` with exactly:
```make
VIA_ENABLE = yes
ENCODER_MAP_ENABLE = yes
```

- [ ] **Step 3: Create `keymap.c` as a copy of the `via` one**

Create `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c` with exactly:
```c
/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_90_ansi(
        KC_MUTE,    KC_ESC,   KC_BRID,  KC_BRIU,  KC_MICT,  KC_LAPA,  RGB_VAD,   RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_INS,             KC_DEL,
        MC_1,       KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        MC_2,       KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
        MC_3,       KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
        MC_4,       KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,
        MC_5,       KC_LCTL,  KC_LOPTN,           KC_LCMMD, KC_SPC,  MO(MAC_FN),           KC_SPC,             KC_RCMMD,           KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_90_ansi(
        RGB_TOG,    _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,            _______,
        _______,    _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,    RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,    _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,   _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,    _______,            _______,  _______,  _______,  _______,   BAT_LVL,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,    _______,  _______,            _______,  _______,  _______,             _______,            _______,            _______,            _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_90_ansi(
        KC_MUTE,    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_INS,             KC_DEL,
        MC_1,       KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        MC_2,       KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
        MC_3,       KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
        MC_4,       KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,
        MC_5,       KC_LCTL,  KC_LWIN,            KC_LALT,  KC_SPC,  MO(WIN_FN),           KC_SPC,             KC_RALT,            KC_RCTL,            KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_90_ansi(
        RGB_TOG,    _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,   RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,   KC_VOLU,  _______,            _______,
        _______,    _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,
        _______,    RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,   _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,
        _______,    _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,   _______,  _______,  _______,  _______,  _______,  _______,             _______,            _______,
        _______,    _______,            _______,  _______,  _______,  _______,   BAT_LVL,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,   _______,  _______,  _______,
        _______,    _______,  _______,            _______,  _______,  _______,             _______,            _______,            _______,             _______,  _______,  _______),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [MAC_FN]   = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
    [WIN_BASE] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [WIN_FN]   = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
};
#endif // ENCODER_MAP_ENABLE
```

- [ ] **Step 4: Compile the new keymap**

Run:
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
make keychron/k15_pro/ansi_encoder/rgb:danilo
```
Expected: build completes with no `ERROR` lines, ending in something like `Copying keychron_k15_pro_ansi_encoder_rgb_danilo.bin to qmk_firmware folder` and exit code 0.

- [ ] **Step 5: Commit**

```bash
cd /Users/danilosandrade/workspace/qmk_firmware
git add keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/rules.mk
git commit -m "Add danilo keymap for K15 Pro RGB as a copy of via"
```

---

## Task 3: Add Flow Tap and Permissive Hold

**Files:**
- Create: `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h`

**Interfaces:**
- Consumes: the `danilo` keymap created in Task 2 (must already compile).
- Produces: `config.h` defining `FLOW_TAP_TERM` and `PERMISSIVE_HOLD`. Task 4 edits this same file to add `CHORDAL_HOLD`.

- [ ] **Step 1: Create `config.h`**

Create `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h` with exactly:
```c
#pragma once

/* Flow Tap: evita que mod-tap resolva como "hold" quando a tecla anterior
 * foi digitada rápido (ex: 'a' + 'space' em sequência de digitação normal) */
#define FLOW_TAP_TERM 150

/* Permissive Hold: combos legítimos entre mãos opostas (ex: A-hold + K)
 * resolvem como hold assim que a outra tecla é solta, sem esperar o
 * tapping term inteiro */
#define PERMISSIVE_HOLD
```

- [ ] **Step 2: Compile**

Run:
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
make keychron/k15_pro/ansi_encoder/rgb:danilo
```
Expected: build completes with no `ERROR` lines, same success output pattern as Task 2 Step 4.

- [ ] **Step 3: Commit**

```bash
cd /Users/danilosandrade/workspace/qmk_firmware
git add keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h
git commit -m "Enable Flow Tap and Permissive Hold on K15 Pro danilo keymap"
```

---

## Task 4: Add Chordal Hold and the hand-designation table

**Files:**
- Modify: `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h`
- Modify: `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c`

**Interfaces:**
- Consumes: `config.h` from Task 3 (adds one more define to it), `keymap.c` from Task 2 (adds one array to it).
- Produces: the finished `danilo` keymap, ready to flash in Task 5.

- [ ] **Step 1: Add the `CHORDAL_HOLD` define**

In `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h`, change:
```c
#define PERMISSIVE_HOLD
```
to:
```c
#define PERMISSIVE_HOLD

/* Chordal Hold: rede de segurança extra — só permite "hold" quando as duas
 * teclas envolvidas são de mãos opostas; combos na mesma mão (rolls) sempre
 * viram tap */
#define CHORDAL_HOLD
```

- [ ] **Step 2: Add the `chordal_hold_layout` hand-designation table**

In `keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c`, the table mirrors the exact key order of the `LAYOUT_90_ansi(...)` calls above: `'L'` for left-hand keys, `'R'` for right-hand keys, `'*'` (wildcard/neutral) for the function row, number row, the `MC_1`-`MC_5` macro column, the navigation cluster (arrows/Home/PgUp/PgDn), and the center `MO(MAC_FN)`/`MO(WIN_FN)` key. The split on alphabetic rows follows the standard touch-typing boundary between the `T`/`G`/`B` column (left) and the `Y`/`H`/`N` column (right); the left and right physical Space keys are `'L'` and `'R'` respectively.

Change:
```c
};

#if defined(ENCODER_MAP_ENABLE)
```
to:
```c
};

// clang-format off
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT_90_ansi(
    '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
    '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
    '*','L','L','L','L','L','L','R','R','R','R','R','R','R','R','*',
    '*','L','L','L','L','L','L','R','R','R','R','R','R','R','*',
    '*','L','L','L','L','L','L','L','R','R','R','R','R','R','*',
    '*','L','L','L','L','*','R','R','R','*','*','*'
);
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
```

- [ ] **Step 3: Compile**

Run:
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
make keychron/k15_pro/ansi_encoder/rgb:danilo
```
Expected: build completes with no `ERROR` lines. If you see an error mentioning `chordal_hold_layout` (e.g. array size mismatch), re-check that the table has exactly 16, 16, 16, 15, 15, 12 entries per row in that order — those counts must match the argument count of each row in the `LAYOUT_90_ansi(...)` calls in the same file.

- [ ] **Step 4: Commit**

```bash
cd /Users/danilosandrade/workspace/qmk_firmware
git add keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/config.h keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/keymap.c
git commit -m "Enable Chordal Hold with hand-designation table on K15 Pro danilo keymap"
```

---

## Task 5: Flash and manually verify on hardware

**Files:**
- None — this task flashes the binary built in Task 4 and runs the manual verification checklist from the design spec.

**Interfaces:**
- Consumes: the compiled `danilo` firmware from Task 4.
- Produces: a keyboard running the new firmware, confirmed to fix the reported issue without breaking Launch compatibility or the deliberate GUI+Space combo.

- [ ] **Step 1: Back up the current VIA keymap from Keychron Launch**

In the Keychron Launch app, export/download the current keymap (the same way `Keymap-K15 Pro RGB-9-19-36.json` was generated). Save it somewhere safe. This is a precaution in case the dynamic keymap in EEPROM ever needs to be restored after flashing.

- [ ] **Step 2: Put the keyboard into bootloader (DFU) mode**

Per `keyboards/keychron/k15_pro/readme.md`: connect the USB cable, toggle the keyboard's mode switch to "Off", hold down the *Esc* key (or the reset button underneath the space bar), then toggle the mode switch to "Cable" while still holding the key/button.

- [ ] **Step 3: Flash the firmware**

Run:
```bash
cd /Users/danilosandrade/workspace/qmk_firmware
make keychron/k15_pro/ansi_encoder/rgb:danilo:flash
```
Expected: `dfu-util` detects the device and reports a successful download/flash, ending in something like `dfu-util: Done!` with exit code 0. The keyboard re-enumerates as a normal USB HID device after flashing.

- [ ] **Step 4: Confirm Keychron Launch still works**

Open Keychron Launch, confirm it detects the keyboard, and confirm the previously configured home row mods (and any other custom keys) are still present. If they were reset, re-import the backup from Step 1.

- [ ] **Step 5: Verify the fast-typing fix**

Type words ending in fast "a " sequences and other home-row-mod letters in quick succession (e.g. "da ", "fa ", "asdf") in a plain text field on macOS. Confirm Spotlight does **not** open and no unwanted modifier fires.

- [ ] **Step 6: Verify the deliberate combo still works**

Hold `A` past roughly 200ms, then press Space. Confirm Spotlight **does** open — the deliberate GUI+Space combo must still work.

- [ ] **Step 7: Verify general typing feel**

Type a full paragraph of normal text at a natural pace. Confirm there's no perceptible added latency or "sticky" feeling on any key.

No commit for this task — it is hardware verification, not a code change. If Steps 5-7 reveal issues, note which one and adjust `FLOW_TAP_TERM` (Task 3) or the `chordal_hold_layout` table (Task 4) accordingly, then repeat Tasks 3/4's compile step and this task's flash+verify steps.
