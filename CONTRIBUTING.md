# Contributing

## What is local, and what is upstream

Almost everything under `lib/` and `app/` is upstream code from the
[ARINC 615A Tool Suite](https://git.thomas-vogt.de/thomas-vogt/arinc_615a) by
Thomas Vogt, carried here unmodified. Keep it that way — the value of this
repository is that a change upstream can be merged without a conflict resolution
pass through five thousand lines of Qt.

| Path | Origin | Change it? |
| --- | --- | --- |
| `lib/arinc_615a/` | upstream, vendored core | **No.** Fix it upstream, or in the CLI repository if the two are being kept in step. |
| `lib/arinc_615a_qt/` | upstream | Only for a defect. Note it below. |
| `lib/arinc_615a_dla_qt/` | upstream | Only for a defect. Note it below. |
| `app/arinc_615a_data_loader_gui/` | upstream | Only for a defect. Note it below. |
| `CMakeLists.txt`, `lib/`, `app/` CMake | **local** | Freely. |
| `cmake/`, `scripts/`, `build.bat` | **local** | Freely. |
| `docs/`, `README.md` | **local** | Freely. |

### Local changes that must survive an upstream merge

These are the deliberate divergences from upstream. If you re-vendor, re-apply
them.

1. **Top-level `CMakeLists.txt` is a rewrite, not a patch.** Upstream's builds
   both sections and treats Qt as optional. This one declares
   `find_package( Qt6 REQUIRED COMPONENTS Widgets Network )`, adds only the
   graphical section, and does not add `doc/`.

2. **`cmake/InstallPackage.cmake` lists the GUI target set.** Upstream's `LIBS`
   names `arinc_615a_commands`. A `$<TARGET_FILE_DIR:...>` on a target that was
   never added is a hard configure error, so the list here names
   `arinc_615a_qt` and `arinc_615a_dla_qt` instead.

3. **`CMAKE_COMPILE_WARNING_AS_ERROR` is `False` in the presets.** Upstream sets
   it `True`. AUTOMOC and AUTOUIC generated translation units and the Qt headers
   do not survive `/W4 /WX` with MSVC 14.44, and Boost 1.92's exception headers
   raise C4127 through `/external:templates-`. Turning this back on will break
   the build for reasons that have nothing to do with this repository's code.

4. **`scripts/env-gui.bat` overrides `VCPKG_ROOT` after `vcvars64.bat`.** This
   is not paranoia — `vcvars64.bat` injects its own, pointing at Visual
   Studio's bundled vcpkg, and also puts CMake 3.31 ahead of the required 4.3
   on `PATH`. Both are worked around there; see the README's
   *Two environment traps*.

---

## Layout rules

- CMake helper files live in `cmake/`, presets in `cmake/presets/`.
- Wrapper scripts live in `scripts/`. `build.bat` at the root is the only
  entry point that chains them.
- Documentation lives in `docs/`. `README.md` is the front door and should stay
  readable end to end without following a link.
- Screenshots go in `docs/figures/` as PNG.

## Line endings

`.gitattributes` is inherited from upstream and normalises text files to LF in
the repository. Git will convert on checkout under Windows; do not fight it, and
do not commit a file with mixed endings.

## Code style

`.clang-format` and `.clang-tidy` are upstream's. Match the surrounding code:
the project uses `V` suffixes on private members, brace initialisation
throughout, and Doxygen `@brief` blocks on every declaration. Run clang-format
before committing anything under `lib/` or `app/`.

Generated sources are exempt from clang-tidy — both Qt libraries write a stub
`.clang-tidy` into their binary directory to silence the checker on AUTOMOC and
AUTOUIC output. Do not remove it.

## Regenerating the documents

`docs/CODE-TRACE.md` is **generated**. The HTML in `docs/code-trace-html/` is
the source of truth:

```bash
python docs/render-code-trace.py
```

Edit the HTML, re-render, and commit both. A hand-edit to `CODE-TRACE.md` will
be silently overwritten by the next run.

## Before opening a pull request

1. `build.bat --no-run` completes with no errors.
2. The application starts and a FIND query completes — see the README's
   *Smoke test*.
3. If you touched the trace HTML, `python docs/render-code-trace.py` produced no
   unexpected diff.
4. If you touched anything under `lib/` or `app/`, say in the description why it
   could not be fixed upstream instead.
