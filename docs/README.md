# Documentation map

Six documents. Start at the row that matches what you are trying to do.

| I want to… | Read | Time |
| --- | --- | --- |
| **Build and run it, now** | [../README.md → Quick start](../README.md#quick-start--one-command) | 2 min |
| **Understand what ARINC 615A is and how it works on a network** | [../README.md → How it works](../README.md#how-it-works--the-layers) | 10 min |
| **Decide between this and the CLI** | [../README.md → CLI or GUI](../README.md#cli-or-gui--which-do-you-want) | 2 min |
| **Fix a build that is failing** | [BUILD.md](BUILD.md) | 5 min |
| **Install it on an air-gapped machine** | [OFFLINE-INSTALL.md](OFFLINE-INSTALL.md) | 20 min |
| **Understand the codebase before changing it** | [ARCHITECTURE.md](ARCHITECTURE.md) | 30 min |
| **Follow one operation through the code, line by line** | [CODE-TRACE.md](CODE-TRACE.md) | 1.5 h |
| **Read or circulate that trace as a document** | [code-trace-html/arinc615a-gui-engineering.html](code-trace-html/arinc615a-gui-engineering.html) | 1.5 h |

---

## The documents

### [ARCHITECTURE.md](ARCHITECTURE.md) — how the codebase is organised
The layer map, the thread boundary and why it is drawn where it is, what each
directory is responsible for, the adapter pattern that every operation follows,
and the design conventions that are not obvious from reading the files.
**Read this before changing code.**

### [CODE-TRACE.md](CODE-TRACE.md) — every function on the path
A trace from `main()` to a byte on the wire and back through the queued signals
that update the widgets. 22 sections, each entry carrying its `file:line`.
Covers the concurrency model, the shell, each of the five operations, the model
layer, resources, persistence, the build, and three thread-affinity defects.
**Reference, not a tutorial** — §07 and §08 are the shared machinery that
§09–§13 all build on, so read them first.

### [code-trace-html/arinc615a-gui-engineering.html](code-trace-html/arinc615a-gui-engineering.html) — the trace, as a page
The same 22 sections, laid out for reading and circulation: index rail,
collapsible function entries, light and dark themes. Self-contained — no
external assets beyond the webfont.

### [OFFLINE-INSTALL.md](OFFLINE-INSTALL.md) — installing without a network
The five network dependencies and how to remove each one, the transfer bundle
with sizes and a checksum manifest, the procedure split across the connected and
the air-gapped machine, how to *prove* the result is genuinely offline, and the
vcpkg ABI caveat that decides whether a prepared `vcpkg_installed` is reusable
at all. **Read it before preparing media**, not after.

### [BUILD.md](BUILD.md) — building, in detail
Every build stage in order, the two ways to obtain Qt and why one is chosen, the
`vcvars64.bat` environment traps, the deployment steps, and the failure modes
that will otherwise stop you.

### [../README.md](../README.md) — the front door
Overview, the layer stack, block and sequence diagrams, quick start, the five
operations, and where the application keeps its state.

---

## Figures

`figures/` holds the screenshots embedded in the README, captured from a debug
build on Windows 11.

| Figure | Subject |
| --- | --- |
| `main-window.png` | Main window — the three statistic group boxes and the three toolbars |
| `about-dialog.png` | About dialogue — dependency versions and licences of a real build |
| `gui-find-wizard-address.png` | FIND wizard page 1 — broadcast address selection |
| `gui-find-wizard-results.png` | FIND wizard page 2 — query running, *Abort Operation* live |
| `gui-find-wizard-complete.png` | FIND wizard page 2 — query complete, *Finish* enabled |
| `gui-main-window.png` | Main window before any operation |
| `gui-main-after-find.png` | Main window after one FIND query — TX counter populated |
| `release-bundle-running.png` | The published release bundle running with a minimal `PATH` |

---

## Reading order for a new maintainer

1. [../README.md](../README.md) — what it is, and build it once.
2. [ARCHITECTURE.md](ARCHITECTURE.md) — the shape of the code, especially the
   thread boundary.
3. [CODE-TRACE.md](CODE-TRACE.md) §04–§08 — concurrency, startup, the adapter
   pattern and the wizard skeleton, in order.
4. [CODE-TRACE.md](CODE-TRACE.md) §09 (FIND) — the simplest complete operation.
5. Whichever of §10–§13 you need.
6. [../CONTRIBUTING.md](../CONTRIBUTING.md) — the local changes that must
   survive an upstream merge.

---

## Relationship to the CLI repository

[arinc-615a-cli-tool-suite](https://github.com/Hitheshkaranth/arinc-615a-cli-tool-suite)
documents the same protocol core from the command-line side. Its
`docs/CODE-TRACE.md` §09–§17 cover the wire layers — the protocol file codec,
the TFTP shim, the timers, the status codes — in more depth than this repository
does, because those layers are shared and unchanged. Read it for anything below
the adapter layer.
