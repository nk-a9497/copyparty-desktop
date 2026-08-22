# Copyparty port — feasibility notes

Status: feasibility investigation (2026-08-22), source = `opencloud-eu/desktop` @ main (shallow). License: GPL-2.0 (port permitted; derived work must stay GPL-2.0).

## Goal
Point the OpenCloud Desktop client at a plain **copyparty** WebDAV server
(`https://cloud.jkodjib.us`, HTTP Basic auth), with **on-demand sync**: files stay
cloud-only; the user right-clicks a folder/file → "save locally" (download), or
"free up space" (placeholder). No forced background two-way sync.

## Verdict: FEASIBLE, moderate effort, two big required changes + several subsystems to disable

### 1. Connection / account setup — MUST REPLACE (the blocker)
Today the wizard requires OpenCloud-only JSON endpoints:
- `connectionvalidator` -> `StatusNotFound` probe (status.php family)
- `fetchserversettings.cpp:55` -> `JsonApiJob` GET `ocs/v2.php/cloud/capabilities`
- `src/cmd/cmd.cpp:472` -> `ocs/v1.php/cloud/capabilities`
- Followed by the OpenCloud oIDC login flow.

copyparty offers none of these (only WebDAV + HTTP Basic). Required change:
add a "Plain WebDAV / copyparty" connection type that:
- verifies the server with a plain authenticated PROPFIND (not status.php/ocs),
- uses HTTP Basic credentials instead of the login flow,
- snapshots the DAV root as a single "space" (no space registry).

### 2. Sync protocol driver — MUST REPLACE (the other big block)
The engine (`src/libsync/discovery.cpp`, `propagate*.cpp`) is built on:
- server-provided **etags** + `oc:` PROPFIND props (fileid, checksums, favourite, permissions) — copyparty does not provide them,
- spaces endpoints `/dav/spaces/<id>` — absent on copyparty,
- direct PUT (copyparty versions existing files — need DELETE-then-PUT),
- recursive listings that hide `.obsidian/` (treeview: copyparty omits dot-prefixed dirs) — must treat "not listed" as "unknown, don't delete",
- WebDAV modtime precision ~1 year — mtime deltas are unusable; same-size detection must be size-only.

This maps 1:1 onto the quirks already handled by the project's custom rclone-based
engine (size+missing push, deletion gate, delete-then-PUT, no mtime same-size).
The port = re-implement that driver inside the client's discover/propagate pipeline.

### 3. Reusable as-is (the value of this fork)
- **VFS + pin states** — `src/plugins/vfs/cfapi` (Windows Cloud Files: placeholders,
  hydration/dehydration) + `PinState`/`setPinState` + `make-available-offline`
  right-click (socketapi) — this IS the requested UX.
- Entire **GUI/tray/account/conflict/settings** stack (`src/gui`, `src/libsync/common`).
- Ignore rules, conflict rename handling, logging.

### 4. Subsystems to disable (server-dependent)
ocs sharing, activity/notifications feed, thumbnails/preview, app-providers,
quota from capabilities, auto-update channel (keep), "file locking" UI.

## Suggested implementation order
1. Fork hooked: connection-type "Plain WebDAV" (PROPFIND verify + Basic auth + root-as-space).
2. Wire VFS plugin selection to always use `cfapi` (Windows) for on-demand; keep `off` as fallback.
3. Replace the discovery/propagate driver with the size+missing / delete-then-PUT engine (port of the existing rclone-based engine, pure HTTP in C++).
4. Null out (disable) the ocs/activity/thumbnails/appprovider code paths.
5. Shell integration: install "Available offline" / "Free up space" context menu (already in socketapi shell_integration).

## Cross-check: why not keep the rclone/Go tray app instead
Current production setup (`CloudShare.exe`, this org) already delivers the same
end-user result via rclone WinFsp mounts: obsidian always local; other folders are
on-demand placeholders; proxy auto-switch; no server-API coupling. Trade-offs:
- Go tray app: works today, minimal surface, ~zero maintenance; VFS = WinFsp whole-folder (not per-file pin).
- OpenCloud port: native per-file pin/free-up-space + polished GUI, but a multi-week port that then needs ongoing tracking of upstream changes.

Recommendation: treat this fork as the long-term path (start the port when time
allows); keep the Go app as the working solution meanwhile.

## Build notes
- CMake + Qt6 (+ ECM), submodules for 3rdparty; cross-compiling Windows from Linux
  is possible but Qt-windows toolchain heavy — prefer building on Windows/WSL or
  container with the craft/ecm setup referenced in PACKAGING.md.
