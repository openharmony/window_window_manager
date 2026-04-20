# AGENTS.md — Window Manager (OpenHarmony)

This repo is a window sussystem of OpenHarmony. 
The Window Manager subsystem provides basic capabilities of window and display management. It is the basis for UI display. The following figure shows the architecture of the Window Manager subsystem.

## What is `window_manager`
You can read [README](./README.md) to obtain detail information.

## WorkFlow

### CodeStyle specification
You must follow the [CodeStyle](./docs/CodeStyle.md) specification when you write code.

### Testing specification
You must follow the [Testing](./docs/Testing.md) specification after you write testing code and keep testing suite pass.

### Git Commit Rules

- **User approval required**: Ask user before `git commit`. Use `git commit -s` after approval.
- **Angular format**: `type(scope): subject` (feat, fix, docs, style, refactor, test, chore)
- **Co-authored footer**: Append `Co-Authored-By: Agent` to every commit message.

Example:
```
feat(auth): add user login feature

Signed-off-by: Your Name <your.email@example.com>
Co-Authored-by: Agent
```
