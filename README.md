# Lab 08 – Memory-Mapped I/O & Polling

CS2053 – Computer Architecture  
Department of Computer Science and Engineering, University of Moratuwa

## Overview

- Memory-mapped I/O (MMIO) architecture and device register addressing
- Peripheral interfacing in Ripes (LED matrices, D-pad, switches, and console display)
- Status register polling (busy-waiting) loops vs interrupt-driven device notification
- Driver-level hardware abstraction routines in RISC-V assembly

## Theory Alignment

- Canonical lecture concepts, ISA specifications, and architecture theory are documented in the [CS2053-Weekly-Notes](https://github.com/CS2053-Computer-Architecture/CS2053-Weekly-Notes) repository.

## Repository Structure

```text
.
├── src/
├── io-configs/
├── evidence/
└── README.md
```

## Engineering & Git Standards

- **Commit Conventions**: All commits follow the Conventional Commits format (`feat:`, `fix:`, `docs:`, `chore:`, `refactor:`).
- **Branching Workflow**: Assembly programs, processor simulations, and analysis tasks utilize feature branches prior to merging into `main`.
- **Reproducibility**: Register trace tables, Ripes simulation screenshots, and benchmark logs are preserved in `evidence/`.

## Academic Context

Department of Computer Science and Engineering, University of Moratuwa.
