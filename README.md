# DynamoRIO AArch64 reg-spill translate MRE

This is a minimal reproducer for the AArch64 debug assertion:

```text
core/translate.c:334 spill_or_restore && r == reg - REG_START_SPILL && !spill && spill_tls
```

It contains:

- `cbr_stolen.S`: tiny static AArch64 app with `mov x0, x28; b _start`.
- `regspill_mre.c`: DynamoRIO client that adds an inline store to address `0x1`, and uses `drreg` so the taken path has ordinary DR TLS spills.

You can build and run this using `./run.sh`. The `logs/` folder will contain logs generated from those runs.

