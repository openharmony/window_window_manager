# AGENTS.md — Window Manager (OpenHarmony)

This repo is the window management subsystem of OpenHarmony. It provides core capabilities for window and display management, serving as the foundational subsystem for UI display. See [README.md](./README.md) for full architecture details.

## What is `window_manager`

`window_manager` is a `Client-Server` architecture subsystem. Key modules:
- `wm/` — Window Manager Client
- `dm/` — Display Manager Client
- `wmserver/` — Window Manager Server (separated arch)
- `dmserver/` — Display Manager Server
- `window_scene/` — Window Manager Server (unified arch)
- `interfaces/innerkits/` — Native APIs
- `interfaces/kits/` — JS/NAPI APIs
- `extension/` — Ability Component window integration
- `utils/` — Shared utilities

Two compile-time architectures are selected via `window_manager_use_sceneboard` in `product/define.gni`:

| Value | Architecture | Key module compiled |
|-------|-------------|---------------------|
| `false` | Separated | `wmserver/` |
| `true` | Unified | `window_scene/` |

## WorkFlow

### CodeStyle specification
You must follow the [CodeStyle](./docs/CodeStyle.md) specification when you write code.

Key rules:
- **C++ standard**: C++11; **Column limit**: 120; **Indent**: 4 spaces, no tabs
- **Namespace**: always `OHOS::Rosen`
- **Naming**: Classes `PascalCase`, methods `camelCase`, member vars `camelCase_`, constants `UPPER_SNAKE_CASE`, files `snake_case`
- **Include order**: corresponding header → stdlib → OH framework → `interfaces/` → other internal
- **Logging**: `TLOGD` / `TLOGI` / `TLOGW` / `TLOGE`
- **Error handling**: return `WMError`/`WmErrorCode`; early-return with `WLOGFE` log
- **Memory**: `sptr<T>` for IPC/singletons, `wptr<T>` for weak refs; prefer RAII
- Every source file must begin with the Apache 2.0 license header

### Testing specification
You must follow the [Testing](./docs/Testing.md) specification after you write testing code and keep testing suite pass.

Key rules:
- Test files use `*_test.cpp` suffix, named after the class under test
- Use `HWTEST_F` macro; namespace must be `OHOS::Rosen`
- `EXPECT_*` for non-fatal checks; `ASSERT_*` for fatal

Build targets per module:

| Module | Build target |
|--------|-------------|
| wm | `wm:test` |
| wmserver | `wmserver:test` |
| dm | `dm:test` |
| dmserver | `dmserver:test` |
| dm_lite | `dm_lite:test` |
| window_scene | `window_scene:test` |
| snapshot | `snapshot:test` |
| extension/window_extension | `extension/window_extension:test` |

