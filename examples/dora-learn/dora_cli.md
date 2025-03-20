
## Dora-cli

```
Commands:
  check        Check if the coordinator and the daemon is running
  graph        Generate a visualization of the given graph using mermaid.js. Use --open to open browser
  build        Run build commands provided in the given dataflow
  new          Generate a new project or node. Choose the language between Rust, Python, C or C++
  run          Run a dataflow locally
  up           Spawn coordinator and daemon in local mode (with default config)
  destroy      Destroy running coordinator and daemon. If some dataflows are still running, they will be stopped first
  start        Start the given dataflow path. Attach a name to the running dataflow by using --name
  stop         Stop the given dataflow UUID. If no id is provided, you will be able to choose between the running dataflows
  list         List running dataflows
  logs         Show logs of a given dataflow and node
  daemon       Run daemon
  runtime      Run runtime
  coordinator  Run coordinator
  help         Print this message or the help of the given subcommand(s)

Options:
  -h, --help     Print help
  -V, --version  Print version
```
