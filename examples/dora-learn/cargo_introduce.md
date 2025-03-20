
## Cargo.toml

- `[workspace]` — Defines a workspace.
    - `resolver` — Sets the dependency resolver to use.
    - `members` — Packages to include in the workspace.
    - `exclude` — Packages to exclude from the workspace.
    - `default-members` — Packages to operate on when a specific package wasn’t selected.
    - `package` — Keys for inheriting in packages.
    - `dependencies` — Keys for inheriting in package dependencies.
    - `lints` — Keys for inheriting in package lints.
    - `metadata` — Extra settings for external tools.
- `[patch]` — Override dependencies.
- `[replace]` — Override dependencies (deprecated).
- `[profile]` — Compiler settings and optimizations.

## [workspace]

```
[workspace]
members = ["member1", "path/to/member2", "crates/*"]
exclude = ["crates/foo", "path/to/other"]
```

## [package]
```
[workspace]

[package]
name = "hello_world" # the name of the package
version = "0.1.0"    # the current version, obeying semver
authors = ["Alice <a@example.com>", "Bob <b@example.com>"]
```

If the `[workspace] section` is added to a `Cargo.toml` that already defines a `[package]`, the package is the root package of the workspace. The workspace root is the directory where the workspace’s `Cargo.toml` is located. The workspace root is the directory where the workspace’s `Cargo.toml` is located.

## [workspace.package]

```
[workspace.package]
version = "1.2.3"
authors = ["Nice Folks"]
description = "A short description of my package"
documentation = "https://example.com/bar"
```

## [workspace.dependencies]

```
[workspace.dependencies]
cc = "1.0.73"
rand = "0.8.5"
regex = { version = "1.6.0", default-features = false, features = ["std"] }
```

## [dev-dependencies]

You can add a `[dev-dependencies]` section to your `Cargo.toml` whose format is equivalent to `[dependencies]`:

```
[dev-dependencies]
tempdir = "0.3"
```

Dev-dependencies are not used when compiling a package for building, but are used for compiling tests, examples, and benchmarks.

These dependencies are not propagated to other packages which depend on this package.

## [[example]]

By default, examples are executable binaries (with a main() function). You can specify the `crate-type field` to make an example be compiled as a library:

```
[[example]]
name = "foo"
crate-type = ["staticlib"]
```

You can run individual executable examples with the `cargo run` command with the `--example <example-name>` option. Library examples can be built with `cargo build` with the `--example <example-name>` option. `cargo install` with the `--example <example-name>` option can be used to copy executable binaries to a common location. Examples are compiled by `cargo test` by default to protect them from bit-rotting. Set `the test field` to true if you have #[test] functions in the example that you want to run with cargo `test`.

```
version.workspace = true
```

Indicates that the version number for this package will be inherited from the workspace configuration. This is useful in monorepo setups where multiple packages share the same version.
